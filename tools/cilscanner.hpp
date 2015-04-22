#ifndef CILSCANNER_HPP
#define CILSCANNER_HPP

#include<iostream>
#include<opencv2/imgproc/imgproc.hpp>
#include<cstdint>
#include<vector>


struct lightsource
{
	cv::Vec3f direction;
	cv::Vec3f intensity;
};

std::ostream& operator<<(std::ostream& out,const lightsource& ls);
std::istream& operator>>(std::istream& in,lightsource& ls);

typedef std::uint64_t lightpattern_t;
typedef std::vector<lightpattern_t> patternsequence;

bool isonelight(lightpattern_t);
int firstbitset(lightpattern_t);

std::istream& operator>>(std::istream& in,patternsequence& ps);
#endif