
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

// gif-recorder include.
#include "event_monitor.hpp"

#ifdef Q_OS_LINUX
// Xlib include.
#include <X11/Xlib.h>
#include <X11/extensions/record.h>
#include <X11/Xlibint.h>
#endif


//
// EventMonitorPrivate
//

struct EventMonitorPrivate {
	EventMonitorPrivate( EventMonitor * parent )
		:	q( parent )
	{
	}

	EventMonitor * q;

#ifdef Q_OS_LINUX
	Display * display = nullptr;
	Display * display_datalink = nullptr;
	XRecordContext context;
	XRecordRange * range = nullptr;

    static void callback( XPointer ptr, XRecordInterceptData * data );
    void handleRecordEvent( XRecordInterceptData * data );
	bool filterWheelEvent( int detail );
#endif
}; // struct EventMonitorPrivate


#ifdef Q_OS_LINUX

void
EventMonitorPrivate::callback( XPointer ptr, XRecordInterceptData * data )
{
	((EventMonitorPrivate*) ptr)->handleRecordEvent( data );
}

void
EventMonitorPrivate::handleRecordEvent( XRecordInterceptData * data )
{
	if( data->category == XRecordFromServer )
	{
		xEvent * event = (xEvent *) data->data;

		switch( event->u.u.type )
		{
			case ButtonPress:
			{
				if( filterWheelEvent( event->u.u.detail ) )
					emit q->buttonPress();
			}
				break;

			case ButtonRelease:
			{
				if( filterWheelEvent( event->u.u.detail ) )
					emit q->buttonRelease();
			}
				break;

			default:
				break;
		}
	}

	XRecordFreeData( data );
}

bool
EventMonitorPrivate::filterWheelEvent( int detail )
{
	return !( detail >= 4 && detail <= 7 );
}

#endif // Q_OS_LINUX


//
// EventMonitor
//

EventMonitor::EventMonitor()
	:	d( new EventMonitorPrivate( this ) )
{
}

EventMonitor::~EventMonitor()
{
#ifdef Q_OS_LINUX
	XRecordFreeContext( d->display, d->context );
	XFree( d->range );
	XCloseDisplay( d->display );
	XCloseDisplay( d->display_datalink );
#endif // Q_OS_LINUX
}

void
EventMonitor::stopListening()
{
#ifdef Q_OS_LINUX
	XRecordDisableContext( d->display_datalink, d->context );
	XSync( d->display_datalink, true );
#endif // Q_OS_LINUX
}

void
EventMonitor::run()
{
#ifdef Q_OS_LINUX
	d->display = XOpenDisplay( nullptr );
	if( !d->display ) return;

	XRecordClientSpec clients = XRecordAllClients;
	d->range = XRecordAllocRange();
	if( !d->range ) return;

	memset( d->range, 0, sizeof( XRecordRange ) );
	d->range->device_events.first = ButtonPress;
	d->range->device_events.last  = ButtonRelease;

	d->context = XRecordCreateContext( d->display, 0, &clients, 1, &d->range, 1 );
	if( !d->context ) return;

	XSync( d->display, true );

	d->display_datalink = XOpenDisplay( nullptr );
	if( !d->display_datalink) return;

	XSync( d->display_datalink, true );

	if( !XRecordEnableContext( d->display, d->context, d->callback, (XPointer) d.data() ) )
		return;
#endif // Q_OS_LINUX
}
