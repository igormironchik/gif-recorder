
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
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QResizeEvent>
#include <QPainter>
#include <QPalette>


//
// MainWindow
//

MainWindow::MainWindow()
	:	QWidget( nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint )
{
	setAttribute( Qt::WA_TranslucentBackground );

	auto vlayout = new QVBoxLayout( this );
	vlayout->setContentsMargins( 0, 0, 0, 0 );
	m_title = new QWidget( this );
	m_title->setAutoFillBackground( true );
	auto layout = new QHBoxLayout( m_title );
	layout->setContentsMargins( 5, 5, 5, 0 );
	m_recordButton = new QToolButton( m_title );
	m_recordButton->setText( tr( "Record" ) );
	layout->addWidget( m_recordButton );
	layout->addItem( new QSpacerItem( 10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );

	vlayout->addWidget( m_title );
	vlayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding ) );
}

MainWindow::~MainWindow()
{
}

void
MainWindow::resizeEvent( QResizeEvent * e )
{
	m_mask = QBitmap( e->size().width(), e->size().height() );

	QPainter p( &m_mask );
	p.setBrush( Qt::color1 );
	p.setPen( Qt::color1 );
	p.drawRect( rect() );
	p.setPen( Qt::color0 );
	p.setBrush( Qt::color0 );
	const auto xy = m_title->rect().bottomLeft() + QPoint( 5, 5 );
	p.drawRect( xy.x(), xy.y(), width() - 10, height() - m_title->rect().height() - 10 );

	setMask( m_mask );

	e->accept();
}

void
MainWindow::paintEvent( QPaintEvent * )
{
	QPainter p( this );
	p.setPen( palette().color( QPalette::Window ) );
	p.setBrush( palette().brush( QPalette::Window ) );

	p.drawRect( rect() );
}
