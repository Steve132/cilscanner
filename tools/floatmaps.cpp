#include<CImg.h>
#include<vector>
#include<stdexcept>
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include"floatmaps.hpp"
using namespace cimg_library;

static const int types[4]={CV_32F,CV_32FC2,CV_32FC3,CV_32FC4};
static const int inversion[4]={0,1,2,3};
	
static void copy_skipped(float* out,const float* in,size_t n,size_t skipout=1,size_t skipin=1)
{
	for(size_t i=0;i<n;i++)
	{
		*out=*in;
		out+=skipout;
		in+=skipin;
	}
}

static CImg<float> to_cimg(const cv::Mat& m)
{
	if(m.dims > 2)
	{
		throw std::runtime_error("Does not support matrices with more than 2 dims (because opencv sucks)");
	}	
	cv::Mat fm;
	
	m.convertTo(fm,types[m.channels()-1]);
	
	CImg<float> cim(fm.cols,fm.rows,1,fm.channels());
	
	size_t n=fm.cols*fm.rows;

	for(int c=0;c<fm.channels();c++)
	{
		int mcoffset=fm.channels()==1 ? c : inversion[c];
		copy_skipped(cim.data(0,0,0,c),fm.ptr<float>()+mcoffset,n,1,fm.channels());
	}
	return cim;
}
static cv::Mat to_mat(const CImg<float>& image)
{
	//if(image.depth()==1 && image.spectrum()==1)
	//{
	//	return cv::Mat(image.height(),image.width(),CV_32F,image.data());
	//}
	cv::Mat m;
	if(image.depth()==1)
	{
		m=cv::Mat(image.height(),image.width(),types[image.spectrum()-1]);
	}
	else
	{
		int dims[3]={image.height(),image.width(),image.depth()};
		m=cv::Mat(3,dims,types[image.spectrum()-1]);
	}
	size_t n=image.width()*image.height()*image.depth();
	for(int c=0;c<m.channels();c++)
	{
		int mcoffset=m.channels()==1 ? c : inversion[c];
		copy_skipped(m.ptr<float>()+mcoffset,image.data(0,0,0,c),n,m.channels());
	}
	return m;
}
floatmap_metadata read_cr2_metadata(const std::string& fn)
{
	std::string newname(tmpnam(NULL));
	std::string command("dcraw -i -v ");
	command+="\"";
	command+=fn;
	command+="\" > ";
	command+=newname;
	int r=system(command.c_str());
	if(r!=0)
	{
		throw std::runtime_error("Some kind of error in dcraw metadata");
	}
	
	std::ifstream datain(newname.c_str());
	std::string line;
	floatmap_metadata md;
	while (std::getline(datain, line))
	{
		std::istringstream iss(line);
		if(line=="")
		{
			continue;
		}
		std::string cmd;
		iss >> cmd;
		if(cmd=="ISO")
		{
			iss >> cmd >> md.iso_speed;
		}
		else if(cmd=="Shutter:")
		{
			//iss >> md.shutter; TODO
		}
		else if(cmd=="Aperture:")
		{
			//iss >> md.aperture
		}
		else if(cmd=="Focal")
		{
			iss >> cmd >> md.focal_length;
		}
	}
	
	remove(newname.c_str());
	
	return md;
}

cv::Mat read_floatmap(const std::string& fn)
{
	CImg<float> image(fn.c_str());
	//std::cerr<<image.width() << ' ' << image.height() << ' ' << image.depth() << ' ' << image.spectrum() << std::endl;
	
	return to_mat(image);
}
void write_floatmap(const std::string& fn,const cv::Mat& fm)
{
	CImg<float> cifm=to_cimg(fm);
	cifm.save_pfm(fn.c_str());
}
void display_floatmap(const cv::Mat& fm)
{
	CImg<float> cifm=to_cimg(fm);
	cifm.display();
}
cv::Mat bwlabel(const cv::Mat& fm)
{
	if(fm.channels() > 1)
	{
		throw std::runtime_error("Cannot label an image with more than one channel");
	}
	CImg<float> cifm=to_cimg(fm);
	CImg<int32_t> labels(cifm);
	labels.label();
	
	cv::Mat m(labels.height(),labels.width(),CV_32S);
	std::copy(labels.data(0,0,0),labels.data(0,0,0)+labels.height()*labels.width(),m.ptr<int>());
	return m;
}
