
cmake_minimum_required( VERSION 3.19 )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Release"
		CACHE STRING "Choose the type of build."
		FORCE)
endif( NOT CMAKE_BUILD_TYPE )

SET( CMAKE_CXX_STANDARD 14 )

SET( CMAKE_CXX_STANDARD_REQUIRED ON )

project( GifRecorder )

set( BUILD_QGIFLIB_TESTS OFF CACHE INTERNAL "" FORCE )
add_subdirectory( 3rdparty/qgiflib )

add_subdirectory( src )
