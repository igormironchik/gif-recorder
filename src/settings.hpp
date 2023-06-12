
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

#pragma once

// Qt include.
#include <QDialog>

// GIF recorder include.
#include "ui_settings.h"


//
// Settings
//

//! Settings dialog.
class Settings
	:	public QDialog
{
	Q_OBJECT

public:
	Settings( int fpsValue, bool grabCursorValue, QWidget * parent );
	~Settings() override = default;

	int fps() const;
	bool grabCursor() const;

private:
	Q_DISABLE_COPY( Settings )

	Ui::Settings m_ui;
}; // class Settings
