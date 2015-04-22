cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-pthread -std=c++11 -fopenmp")

find_package( OpenCV REQUIRED core imgproc)
include_directories(/usr/include/eigen3)

add_library(floatmaps floatmaps.cpp floatmaps.hpp)
target_link_libraries(floatmaps X11 ${OpenCV_LIBS} opencv_highgui)# for cimg_display

add_library(cilscanner cilscanner.cpp cilscanner.hpp)

add_executable(findlightsources
	findlightsources.cpp
)
target_link_libraries(findlightsources floatmaps cilscanner)

add_executable(solvenormals
	solvenormals.cpp
)
target_link_libraries(solvenormals floatmaps cilscanner)
