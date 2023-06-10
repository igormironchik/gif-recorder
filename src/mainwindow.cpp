
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
#include <QToolButton>
#include <QSpacerItem>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QCloseEvent>


//
// MainWindow
//

MainWindow::MainWindow()
	:	QWidget( nullptr, Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus |
				 Qt::WindowTransparentForInput )
{
	setWindowTitle( tr( "GIF Recorder" ) );
	setAttribute( Qt::WA_TranslucentBackground, true );

	m_w = new QWidget( nullptr, Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint );
	auto vlayout = new QVBoxLayout( m_w );
	vlayout->setContentsMargins( 0, 0, 0, 0 );
	auto layout = new QHBoxLayout;
	layout->setContentsMargins( 0, 0, 0, 0 );
	auto button = new QToolButton( this );
	button->setText( tr( "Record" ) );
	layout->addWidget( button );
	layout->addItem( new QSpacerItem( 10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
	vlayout->addLayout( layout );
	vlayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding ) );
	m_w->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
	m_w->resize( 800, button->sizeHint().height() );
	m_w->move( geometry().x(), geometry().y() );
	m_w->show();
}

MainWindow::~MainWindow()
{
	delete m_w;
}

void
MainWindow::moveEvent( QMoveEvent * e )
{
	m_w->move( geometry().x(), geometry().y() );

	e->accept();
}

void
MainWindow::resizeEvent( QResizeEvent * e )
{
	m_w->resize( e->size().width(), m_w->height() );

	e->accept();
}

void
MainWindow::closeEvent( QCloseEvent * e )
{
	m_w->close();

	e->accept();
}

bool
MainWindow::nativeEvent( const QByteArray & eventType, void * message, qintptr * result )
{
	static int i = 0;
	qDebug() << eventType << ++i;
	return false;
}
