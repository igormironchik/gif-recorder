
project( GIFRecorder.App )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

find_package( Qt6Core REQUIRED )
find_package( Qt6Widgets REQUIRED )
find_package( Qt6Gui REQUIRED )

set( SRC main.cpp
	mainwindow.hpp
	mainwindow.cpp
	settings.hpp
	settings.cpp
	settings.ui )

qt6_add_resources( SRC resources.qrc )

if( WIN32 )
	list( APPEND SRC gif-recorder.rc )
endif()

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/bitmap )

add_executable( GIFRecorder.App WIN32 ${SRC} )

target_link_libraries( GIFRecorder.App bitmap Qt6::Widgets Qt6::Gui Qt6::Core )

if( UNIX )
	target_link_libraries( GIFRecorder.App Xfixes X11 )
endif()
