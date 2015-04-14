#include<vector>
#include<string>
#include<iostream>
#include<stdexcept>
#include<cmath>
#include<opencv2/imgproc/imgproc.hpp>
#include"floatmaps.hpp"
#include<algorithm>
#include <X11/Xlib.h>
#include <CL/cl_platform.h>
using namespace std;

struct circle
{
	double x;
	double y;
	double r;
};

ostream& operator<<(ostream& out,const circle& circ)
{
	return out << circ.x << ' ' << circ.y << ' ' << circ.r;
}

cv::Mat whitebalance(const cv::Mat& imgin)
{
	cv::Mat img=imgin.clone();
	vector<cv::Mat> rgb(3);
	vector<cv::Mat> rgbeq(3);
	cv::split(img,rgb);
	std::vector<float> pxcopy(img.rows*img.cols);
	for(int c=0;c<3;c++)
	{
		cv::medianBlur(rgb[c],rgbeq[c],5); // not 5?
		//cv::GaussianBlur(rgb[c], rgbeq[c], cv::Size(5, 5), 2, 2, cv::BORDER_REFLECT);

		float* start=(float*)rgbeq[c].data;
		size_t n=img.rows*img.cols;
		copy(start,start+n,pxcopy.begin());
		std::nth_element(pxcopy.begin(),pxcopy.begin()+n/2,pxcopy.end());
		rgbeq[c]/=pxcopy[n/2]; // divide entire image by the median. (white balance)
	}
	cv::merge(rgbeq,img);
	return img;
}

cv::Mat color_normalize(const cv::Mat& imgin)
{
	cv::Mat out;
	cv::Mat l2;
	vector<cv::Mat> rgb(3);
	cv::split(imgin,rgb);
	
	cv::sqrt(rgb[0].mul(rgb[0])+rgb[1].mul(rgb[1])+rgb[2].mul(rgb[2]),l2);
	rgb[0]/=l2;
	rgb[1]/=l2;
	rgb[2]/=l2;
	
	cv::merge(rgb,out);
	return out;
}
cv::Mat dilate(const cv::Mat& imgin,int s)
{
	int dilation_size = s;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
				cv::Size(2*dilation_size + 1, 2*dilation_size+1 ),
				cv::Point2i(dilation_size, dilation_size));
	cv::Mat out;
	cv::dilate(imgin,out,element);
	return out;
}




vector<cv::Vec3f> detect_circles(const cv::Mat& g,cv::Mat imgtarget)
{
	vector<cv::Vec3f> circles;

cv::HoughCircles( g, circles, CV_HOUGH_GRADIENT, 1, g.rows/20, 24, 60, 200, 1600);

  /// Draw the circles detected

  for( size_t i = 0; i < circles.size(); i++ )
  {
	  for(int k=0;k<3;k++)
	  {
		  cout << circles[i][k];
	  }
	  cout << endl;
      cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
	//cv::circle( eq, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
	// circle outline
	//cv::circle( eq, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
	
	cv::circle(imgtarget, center, radius, cv::Scalar(255.0), 3, 8, 0);
   }
	if(circles.size() > 0)
	{
		std::cout << "# CIRCLES FOUND: " << circles.size() << std::endl;
		display_floatmap(imgtarget);
	}
}

int most_common_int(const int* begin,const int* end,int flr=0)
{
	std::multiset<int> ms(begin,end);
	int maxsize=*ms.rbegin();
	int maxcount=0;
	int maxindex=0;
	for(int i=flr;i<=maxsize;i++)
	{
		int curcount=ms.count(i);
		if(curcount > maxcount)
		{
			maxcount=curcount;
			maxindex=i;
		}
	}
	return maxindex;
}

pair<size_t,size_t> bounds_gt(const float* begin,const float* end,float threshold)
{
	size_t n=0;
	pair<size_t,size_t> bnds=make_pair(0,0);
	for(size_t i=0;i<n;i++)
	{
		if(begin[i] > threshold)
		{
			bnds.first=i;
			break;
		}
	}
	for(size_t i=n-1;i>=0;i--)
	{
		if(begin[i] > threshold)
		{
			bnds.second=i;
			break;
		}
	}
	return bnds;
}

circle get_circle(const cv::Mat& imgin)
{
	cv::Mat img=imgin.clone();
	
	img=whitebalance(img);
	
	vector<cv::Mat> rgb;
	cv::split(img,rgb);
	
	cv::Mat rmax=(rgb[0]/(rgb[1]+rgb[2])) > 1.0;
	cv::Mat dilated=dilate(rmax,5);
	cv::Mat labels=bwlabel(dilated);
	
	int biggest_label=most_common_int(labels.ptr<int>(),labels.ptr<int>()+labels.rows*labels.cols,1);
	cv::Mat mask=labels == biggest_label;

	rmax&=mask;
//	display_floatmap(rmax);
	
	rmax.convertTo(rmax,CV_32F);
	cv::Mat row_counts=rmax*cv::Mat(rmax.cols,1,CV_32F,1.0);
	cv::Mat column_counts=cv::Mat(1,rmax.rows,CV_32F,1.0)*rmax;
	
 	float threshold=0.99f;
	pair<size_t,size_t> xbnds=bounds_gt(column_counts.ptr<float>(),column_counts.ptr<float>()+column_counts.cols,threshold);
	pair<size_t,size_t> ybnds=bounds_gt(row_counts.ptr<float>(),row_counts.ptr<float>()+row_counts.rows,threshold);
	
	circle circ;
 	circ.r=(xbnds.second-xbnds.first)/2.0;
	circ.x=xbnds.first+circ.r;
	circ.y=ybnds.first+circ.r;
	
	return circ;
	

	// Briefly attempt segmenting red with vanilla thresholding:
	//display_floatmap(0.8*rgbeq[0] > (rgbeq[1]+rgbeq[2]));
	// Dilate thresholded speckle map from thresholded result?
	
	// Does the white balancing Actually work to take out the background/is this method making the image too grainy to resolve?
	
	// Issue: hough transform might not be able to detect circle since ornament has stick end disconnecting the circle edge
	// Most promising sounds like fixing this ^

	// Attempt segmenting red:
	cv::Mat hsv;
	//cv::GaussianBlur(eq, eq, cv::Size(9, 9), 2, 2, cv::BORDER_REFLECT);
//	display_floatmap(eq);

/*	cv::cvtColor(eq,hsv, CV_BGR2HSV);
	vector<cv::Mat> channels(3);
	cv::split(hsv,channels);
	
//	display_floatmap(hsv);
	cv::Mat t;
	t=channels[1] > 0.35;
	t=t & (channels[0] > 240.0 & channels[0] < 320.0);
	t=t & (channels[2] < 0.4);
	
	int dilation_size = 7;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
				cv::Size(2*dilation_size + 1, 2*dilation_size+1 ),
				cv::Point2i(dilation_size, dilation_size));
	/// Apply the dilation operation

//	display_floatmap(channels[2]);
	cv::Mat g, gray, g2;
//	cv::cvtColor(eq, gray, cv::COLOR_RGB2GRAY);
	t.convertTo(g, CV_8UC1, 255);
	cv::dilate(g, g, element);

//	display_floatmap(0.8*rgbeq[0] > (rgbeq[1]+rgbeq[2]));
	return {0.0,0.0,1.0};*/
}


int main(int argc,char** argv)
{
	try
	{
		vector<string> args(argv,argv+argc);
		for(int i=1;i<args.size();i++)
		{
			circle c=get_circle(read_floatmap(args[i]));
			cout << c << endl;
			
			
		}
		return 0;
	} 
	catch(const std::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}
}
