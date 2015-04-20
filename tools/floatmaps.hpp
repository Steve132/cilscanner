#ifndef FLOATMAPS_HPP
#define FLOATMAPS_HPP

#include<opencv2/opencv.hpp>
#include<string>

struct floatmap_metadata
{
	double iso_speed;
	double shutter;
	double aperture;
	double focal_length;
};

floatmap_metadata read_cr2_metadata(const std::string& fn);
cv::Mat read_floatmap(const std::string& fn);
void write_floatmap(const std::string& fn,const cv::Mat& fm);
void display_floatmap(const cv::Mat& fm);
cv::Mat bwlabel(const cv::Mat& fm);

#endif
