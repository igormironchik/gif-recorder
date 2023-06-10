
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

// Qt include.
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QResizeEvent>
#include <QPainter>
#include <QPalette>


//
// ResizeHandle
//

ResizeHandle::ResizeHandle( Orientation o, QWidget * parent, MainWindow * obj )
	:	QFrame( parent )
	,	m_obj( obj )
	,	m_orient( o )
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

	auto h1 = new ResizeHandle( ResizeHandle::TopLeftBotomRight, this, this );
	grid->addWidget( h1, 0, 0 );
	auto h2 = new ResizeHandle( ResizeHandle::Vertical, this, this );
	grid->addWidget( h2, 0, 1 );
	auto h3 = new ResizeHandle( ResizeHandle::BottomLeftTopRight, this, this );
	grid->addWidget( h3, 0, 2 );

	auto h4 = new ResizeHandle( ResizeHandle::Horizontal, this, this );
	grid->addWidget( h4, 1, 0 );

	m_c = new QWidget( this );
	auto vlayout = new QVBoxLayout( m_c );
	vlayout->setContentsMargins( 0, 0, 0, 0 );
	m_title = new QFrame( m_c );
	m_title->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	m_title->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
	m_title->setAutoFillBackground( true );
	auto layout = new QHBoxLayout( m_title );
	layout->setContentsMargins( 5, 5, 5, 5 );
	m_recordButton = new QToolButton( m_title );
	m_recordButton->setText( tr( "Record" ) );
	layout->addWidget( m_recordButton );
	layout->addItem( new QSpacerItem( 10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
	vlayout->addWidget( m_title );
	m_recordArea = new QWidget( m_c );
	m_recordArea->setAttribute( Qt::WA_TranslucentBackground );
	vlayout->addWidget( m_recordArea );

	grid->addWidget( m_c, 1, 1 );

	auto h5 = new ResizeHandle( ResizeHandle::Horizontal, this, this );
	grid->addWidget( h5, 1, 2 );

	auto h6 = new ResizeHandle( ResizeHandle::BottomLeftTopRight, this, this );
	grid->addWidget( h6, 2, 0 );
	auto h7 = new ResizeHandle( ResizeHandle::Vertical, this, this );
	grid->addWidget( h7, 2, 1 );
	auto h8 = new ResizeHandle( ResizeHandle::TopLeftBotomRight, this, this );
	grid->addWidget( h8, 2, 2 );
}

MainWindow::~MainWindow()
{
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
