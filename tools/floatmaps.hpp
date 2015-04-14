#ifndef FLOATMAPS_HPP
#define FLOATMAPS_HPP

#include<opencv2/opencv.hpp>
#include<string>

cv::Mat read_floatmap(const std::string& fn);
void write_floatmap(const std::string& fn,const cv::Mat& fm);
void display_floatmap(const cv::Mat& fm);
cv::Mat bwlabel(const cv::Mat& fm);

#endif
