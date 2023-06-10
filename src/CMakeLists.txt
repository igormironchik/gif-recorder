
project( GIFRecorder.App )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui REQUIRED)

set( SRC main.cpp
	mainwindow.hpp
	mainwindow.cpp )

qt6_add_resources( SRC resources.qrc )

if( WIN32 )
	list( APPEND SRC gif-recorder.rc )
endif()

include_directories( ${ImageMagick_INCLUDE_DIRS} )

add_executable( GIFRecorder.App ${SRC} )

target_link_libraries( GIFRecorder.App ${ImageMagick_LIBRARIES} Qt6::Widgets Qt6::Gui Qt6::Core )
