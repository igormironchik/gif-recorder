
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
	m_title = new TitleWidget( m_c, this );
	auto layout = new QHBoxLayout( m_title );
	layout->setContentsMargins( 5, 5, 5, 5 );
	m_recordButton = new QToolButton( m_title );
	m_recordButton->setText( tr( "Record" ) );
	layout->addWidget( m_recordButton );
	layout->addItem( new QSpacerItem( 10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
	auto settings = new QToolButton( m_title );
	settings->setIcon( QIcon( ":/img/applications-system.png" ) );
	layout->addWidget( settings );
	connect( settings, &QToolButton::clicked, this, &MainWindow::onSettings );
	auto closeButton = new CloseButton( m_title );
	layout->addWidget( closeButton );
	connect( closeButton, &CloseButton::clicked, this, &QWidget::close );
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
}

void
MainWindow::resizeEvent( QResizeEvent * e )
{
	m_mask = QBitmap( e->size() );
	m_mask.fill( Qt::color1 );

	QPainter p( &m_mask );
	p.setPen( Qt::color0 );
	p.setBrush( Qt::color0 );
	auto h = 6 + m_title->sizeHint().height() + 1;
	p.drawRect( 6, h, e->size().width() - 12, e->size().height() - h - 6 );

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
