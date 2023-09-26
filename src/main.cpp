
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

// Qt include.
#include <QApplication>

// GIF recorder include.
#include "mainwindow.hpp"
#include "event_monitor.hpp"


int main( int argc, char ** argv )
{
	QApplication app( argc, argv );

	QIcon appIcon( QStringLiteral( ":/img/icon_256x256.png" ) );
	appIcon.addFile( QStringLiteral( ":/img/icon_128x128.png" ) );
	appIcon.addFile( QStringLiteral( ":/img/icon_64x64.png" ) );
	appIcon.addFile( QStringLiteral( ":/img/icon_48x48.png" ) );
	appIcon.addFile( QStringLiteral( ":/img/icon_32x32.png" ) );
	appIcon.addFile( QStringLiteral( ":/img/icon_22x22.png" ) );
	appIcon.addFile( QStringLiteral( ":/img/icon_16x16.png" ) );
	app.setWindowIcon( appIcon );

	EventMonitor m;

	MainWindow w( &m );
	w.resize( 800, 600 );
	w.show();

	m.start();

	const auto ret = QApplication::exec();

	m.stopListening();
	m.quit();
	m.wait();

	return ret;
}
