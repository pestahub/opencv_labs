#set(OpenCV_DIR "../OCV_install")
set(CMAKE_BUILD_FILES_DIRECTORY "./build")

set (CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 2.8)

project( Blender )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( blender blender.cpp )
target_link_libraries( blender ${OpenCV_LIBS} )

add_compile_options(/D_USE_MATH_DEFINES)