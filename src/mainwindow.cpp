
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2018 Igor Mironchik

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// GIF recorder include.
#include "mainwindow.hpp"
#include "settings.hpp"

// Qt include.
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QResizeEvent>
#include <QPainter>
#include <QPalette>
#include <QMouseEvent>
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>
#include <QScreen>
#include <QMessageBox>
#include <QRunnable>
#include <QThreadPool>
#include <QCloseEvent>

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#endif

#ifdef Q_OS_WINDOWS
#include <Windows.h>
#endif

// C++ include.
#include <exception>


//
// ResizeHandle
//

ResizeHandle::ResizeHandle( Orientation o, bool withMove, QWidget * parent, MainWindow * obj )
	:	QFrame( parent )
	,	m_obj( obj )
	,	m_orient( o )
	,	m_withMove( withMove )
{
	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	setAutoFillBackground( true );

	switch( o )
	{
		case Horizontal :
		{
			setCursor( Qt::SizeHorCursor );
			setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
		}
			break;

		case Vertical :
		{
			setCursor( Qt::SizeVerCursor );
			setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
		}
			break;

		case TopLeftBotomRight :
		{
			setCursor( Qt::SizeFDiagCursor );
			setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
		}
			break;

		case BottomLeftTopRight :
		{
			setCursor( Qt::SizeBDiagCursor );
			setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
		}
			break;
	}
}

QSize
ResizeHandle::minimumSizeHint() const
{
	return { 5, 5 };
}

QSize
ResizeHandle::sizeHint() const
{
	return { 5, 5 };
}

void
ResizeHandle::mousePressEvent( QMouseEvent * e )
{
	if( e->button() == Qt::LeftButton )
	{
		m_leftButtonPressed = true;
		m_pos = e->globalPosition();
	}

	e->accept();
}

void
ResizeHandle::mouseReleaseEvent( QMouseEvent * e )
{
	if( e->button() == Qt::LeftButton && m_leftButtonPressed )
	{
		handleMouseMove( e );

		m_leftButtonPressed = false;
	}

	e->accept();
}

void
ResizeHandle::mouseMoveEvent( QMouseEvent * e )
{
	if( m_leftButtonPressed )
		handleMouseMove( e );

	e->accept();
}

void
ResizeHandle::handleMouseMove( QMouseEvent * e )
{
	auto delta = e->globalPosition() - m_pos;
	m_pos = e->globalPosition();

	QMargins m = { 0, 0, 0, 0 };

	switch( m_orient )
	{
		case Horizontal :
		{
			if( m_withMove )
				m.setLeft( qRound( -delta.x() ) );
			else
				m.setRight( qRound( delta.x() ) );
		}
			break;

		case Vertical :
		{
			if( m_withMove )
				m.setTop( qRound( -delta.y() ) );
			else
				m.setBottom( qRound( delta.y() ) );
		}
			break;

		case TopLeftBotomRight :
		{
			if( m_withMove )
			{
				m.setTop( qRound( -delta.y() ) );
				m.setLeft( qRound( -delta.x() ) );
			}
			else
			{
				m.setBottom( qRound( delta.y() ) );
				m.setRight( qRound( delta.x() ) );
			}
		}
			break;

		case BottomLeftTopRight :
		{
			if( m_withMove )
			{
				m.setLeft( qRound( -delta.x() ) );
				m.setBottom( qRound( delta.y() ) );
			}
			else
			{
				m.setRight( qRound( delta.x() ) );
				m.setTop( qRound( -delta.y() ) );
			}
		}
			break;

		default :
			break;
	}

	m_obj->setGeometry( m_obj->geometry() + m );
	QApplication::processEvents();
}

//
// Title
//

TitleWidget::TitleWidget( QWidget * parent, MainWindow * obj )
	:	QFrame( parent )
	,	m_obj( obj )
{
	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	setAutoFillBackground( true );
	setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
}

void
TitleWidget::mousePressEvent( QMouseEvent * e )
{
	if( e->button() == Qt::LeftButton )
	{
		m_leftButtonPressed = true;
		m_pos = e->globalPosition();
	}

	e->accept();
}

void
TitleWidget::mouseReleaseEvent( QMouseEvent * e )
{
	if( e->button() == Qt::LeftButton && m_leftButtonPressed )
	{
		handleMouseMove( e );

		m_leftButtonPressed = false;
	}

	e->accept();
}

void
TitleWidget::mouseMoveEvent( QMouseEvent * e )
{
	if( m_leftButtonPressed )
		handleMouseMove( e );

	e->accept();
}

void
TitleWidget::handleMouseMove( QMouseEvent * e )
{
	auto delta = e->globalPosition() - m_pos;
	m_pos = e->globalPosition();

	m_obj->move( m_obj->x() + qRound( delta.x() ),
		m_obj->y() + qRound( delta.y() ) );
	QApplication::processEvents();
}


//
// CloseButton
//

CloseButton::CloseButton( QWidget * parent )
	:	QAbstractButton( parent )
{
	setCheckable( false );

	m_activePixmap = QPixmap( QStringLiteral( ":/img/dialog-close.png" ) );

	auto source = m_activePixmap.toImage();
	QImage target = QImage( source.width(), source.height(), QImage::Format_ARGB32 );

	for( int x = 0; x < source.width(); ++x )
	{
		for( int y = 0; y < source.height(); ++y )
		{
			const auto g = qGray( source.pixel( x, y ) );
			target.setPixelColor( x, y, QColor( g, g, g, source.pixelColor( x, y ).alpha() ) );
		}
	}

	m_inactivePixmap = QPixmap::fromImage( target );

	setFocusPolicy( Qt::NoFocus );
}

QSize
CloseButton::sizeHint() const
{
	return { 16, 16 };
}

void
CloseButton::paintEvent( QPaintEvent * e )
{
	QPainter p( this );

	if( m_hovered )
		p.drawPixmap( rect(), m_activePixmap );
	else
		p.drawPixmap( rect(), m_inactivePixmap );
}

void
CloseButton::enterEvent( QEnterEvent * event )
{
	m_hovered = true;

	update();

	event->accept();
}

void
CloseButton::leaveEvent( QEvent * event )
{
	m_hovered = false;

	update();

	event->accept();
}


//
// MainWindow
//

MainWindow::MainWindow()
	:	QWidget( nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint )
{
	setAttribute( Qt::WA_TranslucentBackground );

	auto grid = new QGridLayout( this );
	grid->setHorizontalSpacing( 1 );
	grid->setVerticalSpacing( 1 );
	grid->setContentsMargins( 0, 0, 0, 0 );

	auto h1 = new ResizeHandle( ResizeHandle::TopLeftBotomRight, true, this, this );
	grid->addWidget( h1, 0, 0 );
	auto h2 = new ResizeHandle( ResizeHandle::Vertical, true, this, this );
	grid->addWidget( h2, 0, 1 );
	auto h3 = new ResizeHandle( ResizeHandle::BottomLeftTopRight, false, this, this );
	grid->addWidget( h3, 0, 2 );

	auto h4 = new ResizeHandle( ResizeHandle::Horizontal, true, this, this );
	grid->addWidget( h4, 1, 0 );

	m_c = new QWidget( this );
	auto vlayout = new QVBoxLayout( m_c );
	vlayout->setContentsMargins( 0, 0, 0, 0 );
	vlayout->setSpacing( 0 );
	m_title = new TitleWidget( m_c, this );
	auto layout = new QHBoxLayout( m_title );
	layout->setContentsMargins( 5, 5, 5, 5 );
	m_recordButton = new QToolButton( m_title );
	m_recordButton->setText( tr( "Record" ) );
	connect( m_recordButton, &QToolButton::clicked, this, &MainWindow::onRecord );
	layout->addWidget( m_recordButton );
	layout->addItem( new QSpacerItem( 10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
	m_msg = new QLabel( m_title );
	layout->addWidget( m_msg );
	layout->addItem( new QSpacerItem( 10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
	m_settingsButton = new QToolButton( m_title );
	m_settingsButton->setIcon( QIcon( ":/img/applications-system.png" ) );
	layout->addWidget( m_settingsButton );
	connect( m_settingsButton, &QToolButton::clicked, this, &MainWindow::onSettings );
	m_closeButton = new CloseButton( m_title );
	layout->addWidget( m_closeButton );
	connect( m_closeButton, &CloseButton::clicked, this, &QWidget::close );
	vlayout->addWidget( m_title );
	m_recordArea = new QWidget( m_c );
	m_recordArea->setAttribute( Qt::WA_TranslucentBackground );
	vlayout->addWidget( m_recordArea );

	grid->addWidget( m_c, 1, 1 );

	auto h5 = new ResizeHandle( ResizeHandle::Horizontal, false, this, this );
	grid->addWidget( h5, 1, 2 );

	auto h6 = new ResizeHandle( ResizeHandle::BottomLeftTopRight, true, this, this );
	grid->addWidget( h6, 2, 0 );
	auto h7 = new ResizeHandle( ResizeHandle::Vertical, false, this, this );
	grid->addWidget( h7, 2, 1 );
	auto h8 = new ResizeHandle( ResizeHandle::TopLeftBotomRight, false, this, this );
	grid->addWidget( h8, 2, 2 );

	m_timer = new QTimer( this );
	connect( m_timer, &QTimer::timeout, this, &MainWindow::onTimer );
}

void
MainWindow::resizeEvent( QResizeEvent * e )
{
	m_mask = QBitmap( e->size() );
	m_mask.fill( Qt::color1 );

	QPainter p( &m_mask );
	p.setPen( Qt::color0 );
	p.setBrush( Qt::color0 );
	auto h = 5 + m_title->sizeHint().height() + 1;
	p.drawRect( 5, h, e->size().width() - 10 - 1, e->size().height() - h - 5 - 1 );

	setMask( m_mask );

	e->accept();
}

void
MainWindow::onSettings()
{
	Settings dlg( m_fps, m_grabCursor, this );

	if( dlg.exec() == QDialog::Accepted )
	{
		m_fps = dlg.fps();
		m_grabCursor = dlg.grabCursor();
	}
}

void
MainWindow::onRecord()
{
	if( m_recording )
	{
		m_recordButton->setText( tr( "Record" ) );
		m_settingsButton->setEnabled( true );
		m_timer->stop();

		auto fileName = QFileDialog::getSaveFileName( this, tr( "Save As" ),
			QStandardPaths::standardLocations( QStandardPaths::PicturesLocation ).first(),
			tr( "GIF (*.gif)" ) );

		if( !fileName.isEmpty() )
		{
			if( !fileName.toLower().endsWith( ".gif" ) )
				fileName.append( ".gif" );

			save( fileName );
		}
		else
			m_frames.clear();
	}
	else
	{
		m_recordButton->setText( tr( "Stop" ) );
		m_settingsButton->setEnabled( false );
		m_timer->start( 1000 / m_fps );
		makeFrame();
	}

	m_recording = !m_recording;
}

void
MainWindow::onTimer()
{
	makeFrame();
}

namespace /* anonymous */ {

Magick::Image
convert( const QImage & img )
{
	Magick::Image mimg( Magick::Geometry( img.width(), img.height() ),
		Magick::ColorRGB( 0.0, 0.0, 0.0 ) );

	const double scale = 1.0 / 256.0;

	mimg.modifyImage();

	Magick::PixelPacket * pixels;
	Magick::ColorRGB mgc;

	for( int y = 0; y < img.height(); ++y)
	{
		pixels = mimg.setPixels( 0, y, mimg.columns(), 1 );

		for( int x = 0; x < img.width(); ++x )
		{
			QColor pix = img.pixel( x, y );

			mgc.red( scale * pix.red() );
			mgc.green( scale * pix.green() );
			mgc.blue( scale * pix.blue() );

			*pixels++ = mgc;
		}

		mimg.syncPixels();
	}

	return mimg;
}

#ifdef Q_OS_LINUX

QImage qimageFromXImage( XImage * xi )
{
	QImage::Format format = QImage::Format_ARGB32_Premultiplied;
	if( xi->depth == 24 )
		format = QImage::Format_RGB32;
	else if( xi->depth == 16 )
		format = QImage::Format_RGB16;

	QImage image = QImage( reinterpret_cast< uchar* >( xi->data ),
		xi->width, xi->height, xi->bytes_per_line, format ).copy();

	if( ( QSysInfo::ByteOrder == QSysInfo::LittleEndian && xi->byte_order == MSBFirst ) ||
		( QSysInfo::ByteOrder == QSysInfo::BigEndian && xi->byte_order == LSBFirst ) )
	{
		for( int i = 0; i < image.height(); ++i)
		{
			if( xi->depth == 16 )
			{
				ushort * p = reinterpret_cast< ushort* >( image.scanLine( i ) );
				ushort * end = p + image.width();
				while( p < end )
				{
					*p = ( ( *p << 8 ) & 0xff00 ) | ( ( *p >> 8 ) & 0x00ff );
					++p;
				}
			}
			else
			{
				uint * p = reinterpret_cast< uint* >( image.scanLine( i ) );
				uint * end = p + image.width();
				while( p < end )
				{
					*p = ( ( *p << 24 ) & 0xff000000 ) | ( ( *p << 8 ) & 0x00ff0000 ) |
						( ( *p >> 8 ) & 0x0000ff00 ) | ( ( *p >> 24 ) & 0x000000ff );
					++p;
				}
			}
		}
	}

	if( format == QImage::Format_RGB32 )
	{
		QRgb * p = reinterpret_cast< QRgb* >( image.bits() );
		for( int y = 0; y < xi->height; ++y )
		{
			for( int x = 0; x < xi->width; ++x )
				p[ x ] |= 0xff000000;
			p += xi->bytes_per_line / 4;
		}
	}

	return image;
}

#endif // Q_OS_LINUX

QPair< QImage, QRect >
grabMouseCursor( const QRect & r, const QImage & i )
{
	QImage cursorImage;
	QPoint cursorPos( -1, -1 );
	int w = 0;
	int h = 0;

#ifdef Q_OS_LINUX
	Display * display = XOpenDisplay( nullptr );

	if( !display )
		return { cursorImage, { cursorPos, QSize( w, h ) } };

	XFixesCursorImage * cursor = XFixesGetCursorImage( display );

	cursorPos = r.intersects( { QPoint( cursor->x - cursor->xhot, cursor->y - cursor->yhot ),
			QSize( cursor->width, cursor->height ) } ) ?
		QPoint( cursor->x - cursor->xhot- r.x(), cursor->y - cursor->yhot - r.y() ) :
		QPoint( -1, -1 );

	std::vector< uint32_t > pixels( cursor->width * cursor->height );

	w = cursorPos.x() != -1 ? cursor->width : 0;
	h = cursorPos.y() != -1 ? cursor->height : 0;

	for( size_t i = 0; i < pixels.size(); ++i )
		pixels[ i ] = cursor->pixels[ i ];

	cursorImage = QImage( (uchar*)( pixels.data() ), w, h,
		QImage::Format_ARGB32_Premultiplied).copy();

	XFree( cursor );

	XCloseDisplay( display );

#elif defined( Q_OS_WINDOWS )
	CURSORINFO cursor = { sizeof( cursor ) };

	if( GetCursorInfo( &cursor ) && cursor.flags == CURSOR_SHOWING )
	{
		ICONINFO info = { sizeof( info ) };

		if( GetIconInfo( cursor.hCursor, &info ) )
		{
			HWND hWnd = GetDesktopWindow();
			HDC hDC = GetWindowDC( hWnd );
			HDC hdcMem = CreateCompatibleDC( hDC );
			BITMAP bmpCursor = { 0 };
			GetObject( info.hbmColor ? info.hbmColor : info.hbmMask,
				sizeof( bmpCursor ), &bmpCursor );
			HBITMAP hBitmap = CreateCompatibleBitmap( hDC, bmpCursor.bmWidth, bmpCursor.bmHeight );
			auto original = SelectObject( hdcMem, hBitmap );

			const QPoint ctl( cursor.ptScreenPos.x - info.xHotspot,
				cursor.ptScreenPos.y - info.yHotspot );
			w = bmpCursor.bmWidth;
			h = bmpCursor.bmHeight;

			for( int x = 0; x < w; ++x )
			{
				for( int y = 0; y < h; ++y )
				{
					const QPoint c = QPoint( x, y ) + ctl;

					if( r.contains( c ) )
					{
						const auto color = i.pixelColor( c - r.topLeft() );
						SetPixel( hdcMem, x, y, RGB( color.red(), color.green(), color.blue() ) );
					}
				}
			}

			DrawIconEx( hdcMem, 0, 0, cursor.hCursor, 0, 0, 0, nullptr, DI_DEFAULTSIZE | DI_NORMAL );

			QImage img( bmpCursor.bmWidth, bmpCursor.bmHeight, QImage::Format_ARGB32 );
			img.fill( Qt::transparent );

			for( int x = 0; x < w; ++x )
			{
				for( int y = 0; y < h; ++y )
				{
					const QPoint c = QPoint( x, y ) + ctl;

					if( r.contains( c ) )
					{
						const auto winColor = GetPixel( hdcMem, x, y );
						const auto color1 = i.pixelColor( c - r.topLeft() );
						const auto color2 = QColor( GetRValue( winColor ),
							GetGValue( winColor ), GetBValue( winColor ) );

						if( color1 != color2 )
							img.setPixelColor( x, y, color2 );
					}
				}
			}

			cursorPos = r.intersects( { ctl, QSize( w, h ) } ) ?
				ctl - r.topLeft() :	QPoint( -1, -1 );
			w = cursorPos.x() != -1 ? w : 0;
			h = cursorPos.y() != -1 ? h : 0;
			cursorImage = img.copy();

			if( info.hbmMask )
				DeleteObject( info.hbmMask );

			if( info.hbmColor )
				DeleteObject( info.hbmColor );

			SelectObject( hdcMem, original );

			DeleteDC( hdcMem );
			DeleteObject( hBitmap );
		}

		if( cursor.hCursor )
			DeleteObject( cursor.hCursor );
	}
#endif

	return { cursorImage, { cursorPos, QSize( w, h ) } };
}

} /* namespace anonymous */

void
MainWindow::makeFrame()
{
	const auto p = mapToGlobal( QPoint( m_c->pos().x() - 1, m_c->pos().y() + m_title->height() ) );
	const auto s = QSize( m_recordArea->width() + 2, m_recordArea->height() + 1 );

	try {
		auto qimg = QApplication::primaryScreen()->grabWindow( 0, p.x(), p.y(),
			s.width(), s.height() ).toImage();

		if( m_grabCursor )
		{
			QImage ci;
			QRect cr;
			std::tie( ci, cr ) = grabMouseCursor( QRect( p, s ), qimg );

			QPainter p( &qimg );
			p.drawImage( cr, ci, ci.rect() );
		}

		auto img = convert( qimg );
		img.animationDelay( 100 / m_fps );
		m_frames.push_back( img );
	}
	catch( const Magick::Exception & )
	{
		QMessageBox::critical( this, tr( "Resources exceeded..." ),
			tr( "You were used all available to ImageMagick resources. Recording is stopped." ) );

		onRecord();
	}
}

namespace /* anonymous */ {

class WriteGIF final
	:	public QRunnable
{
public:
	WriteGIF( std::vector< Magick::Image >::iterator first,
		std::vector< Magick::Image >::iterator last,
		const std::string & fileName )
		:	m_first( first )
		,	m_last( last )
		,	m_fileName( fileName )
	{
		setAutoDelete( false );
	}

	~WriteGIF() noexcept override = default;

	void run() override
	{
		try {
			std::vector< Magick::Image > tmp;

			Magick::optimizeImageLayers( &tmp, m_first, m_last );

			Magick::writeImages( tmp.begin(), tmp.end(), m_fileName );
		}
		catch( ... )
		{
			m_eptr = std::current_exception();
		}
	}

	std::exception_ptr exception() const
	{
		return m_eptr;
	}

private:
	std::vector< Magick::Image >::iterator m_first;
	std::vector< Magick::Image >::iterator m_last;
	std::string m_fileName;
	std::exception_ptr m_eptr;
}; // class WriteGIF

} /* namespase anonymous */

void
MainWindow::save( const QString & fileName )
{
	m_recordButton->setEnabled( false );
	m_settingsButton->setEnabled( false );

	m_msg->setText( tr( "Writing GIF... Please wait." ) );

	QApplication::processEvents();

	try {
		m_busy = true;

		WriteGIF runnable( m_frames.begin(), m_frames.end(), fileName.toStdString() );
		QThreadPool::globalInstance()->start( &runnable );

		while( !QThreadPool::globalInstance()->waitForDone( 10 ) )
			QApplication::processEvents();

		m_busy = false;

		if( runnable.exception() )
			std::rethrow_exception( runnable.exception() );
	}
	catch( const Magick::Exception & x )
	{
		QMessageBox::critical( this, tr( "Failed to save GIF..." ),
			QString::fromLocal8Bit( x.what() ) );
	}
	catch( const std::exception & x )
	{
		QMessageBox::critical( this, tr( "Failed to save GIF..." ),
			QString::fromLocal8Bit( x.what() ) );
	}

	m_recordButton->setEnabled( true );
	m_settingsButton->setEnabled( true );

	m_msg->setText( {} );

	m_frames.clear();
}

void
MainWindow::closeEvent( QCloseEvent * e )
{
	if( m_busy )
	{
		const auto btn = QMessageBox::question( this, tr( "GIF recorder is busy..." ),
			tr( "GIF recorder is busy.\nDo you want to terminate the application?" ) );

		if( btn == QMessageBox::Yes )
			exit( -1 );
		else
			e->ignore();
	}
	else
		e->accept();
}
