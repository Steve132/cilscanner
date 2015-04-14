cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-pthread -std=c++11")
find_package( OpenCV REQUIRED core imgproc)
add_library(floatmaps floatmaps.cpp floatmaps.hpp)
target_link_libraries(floatmaps X11 ${OpenCV_LIBS} opencv_highgui)# for cimg_display

add_executable(findred 
	findred.cpp
)
target_link_libraries(findred floatmaps)
