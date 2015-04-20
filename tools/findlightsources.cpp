#include<vector>
#include<string>
#include<iostream>
#include<stdexcept>
#include<cmath>
#include<opencv2/imgproc/imgproc.hpp>
#include"floatmaps.hpp"
#include<algorithm>
using namespace std;

struct circle
{
	double x;
	double y;
	double r;
};

struct lightsource
{
	cv::Vec3f direction;
	cv::Vec3f intensity;
};

ostream& operator<<(ostream& out,const circle& circ)
{
	return out << circ.x << ' ' << circ.y << ' ' << circ.r;
}

ostream& operator<<(ostream& out,const lightsource& ls)
{
	for(int i=0;i<3;i++)
	{
		out << ls.direction[i];
	}
	for(int i=0;i<3;i++)
	{
		out << ls.intensity[i];
	}
	return out;
}

cv::Vec3f median_color(const cv::Mat& imgin)
{
	cv::Vec3f colorout;
	vector<cv::Mat> rgb(3);
	vector<cv::Mat> rgbeq(3);
	cv::split(imgin,rgb);
	std::vector<float> pxcopy(imgin.rows*imgin.cols);
	for(int c=0;c<3;c++)
	{
		cv::medianBlur(rgb[c],rgbeq[c],5); // not 5?
		//cv::GaussianBlur(rgb[c], rgbeq[c], cv::Size(5, 5), 2, 2, cv::BORDER_REFLECT);

		float* start=(float*)rgbeq[c].data;
		size_t n=imgin.rows*imgin.cols;
		copy(start,start+n,pxcopy.begin());
		size_t percent2=(n*2)/100;
		size_t p1=n/2-percent2/2;
		size_t p2=n/2+percent2/2;
		
		std::nth_element(pxcopy.begin(),pxcopy.begin()+n/2,pxcopy.end());
		colorout[c]=pxcopy[n/2];
		
	//	std::nth_element(pxcopy.begin(),pxcopy.begin()+p1,pxcopy.begin()+p2); //linear time create range in the middle that contains middle 2% of pixels
	//	std::nth_element(pxcopy.begin()+p1,pxcopy.begin()+p2,pxcopy.end());
	//	colorout[c]=std::accumulate(pxcopy.begin()+n/2-percent2/2,pxcopy.end()+n/2+percent2/2,0.0f)/(float)percent2;
	}
	return colorout;
}

cv::Mat whitebalance(const cv::Mat& imgin)
{
	cv::Mat img;
	
	cv::Vec3f median=median_color(imgin);
	vector<cv::Mat> rgb(3);
	cv::split(imgin,rgb);
	for(int c=0;c<3;c++)
	{
		rgb[c]/=median[c];
	}
	cv::merge(rgb,img);
	
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

void draw_circle(const circle& c,cv::Mat& target,const cv::Scalar& color)
{
	cv::Point center(cvRound(c.x), cvRound(c.y));
	int radius = cvRound(c.r);
      // circle center
	//cv::circle( eq, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
	// circle outline
	//cv::circle( eq, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
	
	cv::circle(target, center, radius, color, 3, 8, 0);
}

std::vector<cv::Vec2d> find(const cv::Mat& bin)
{
	std::vector<cv::Vec2d> p;
	p.reserve((bin.rows*bin.cols)/1024); 
	for(size_t i=0;i<bin.rows;i++)
	{
		const unsigned char *rowptr=bin.ptr<const unsigned char>(i);
		for(size_t j=0;j<bin.cols;j++)
		{
			if(rowptr[j])
			{
				p.emplace_back((double)i,(double)j);
			}
		}
	}
	return p;
}

cv::Vec2d average_center(const cv::Mat& bin)
{
	std::vector<cv::Vec2d> p=find(bin);
	return std::accumulate(p.cbegin(),p.cend(),cv::Vec2d(0.0,0.0))/(double)p.size();
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
	size_t n=end-begin;
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

cv::Mat remove_smalls(const cv::Mat& m)
{
	cv::Mat labels=bwlabel(m);
	
	int biggest_label=most_common_int(labels.ptr<int>(),labels.ptr<int>()+labels.rows*labels.cols,1);
	return labels == biggest_label;	
}

circle get_circle(const cv::Mat& imgin,bool display=false)
{
	cv::Mat img=imgin.clone();
	
	img=whitebalance(img);
	
	vector<cv::Mat> rgb;
	cv::split(img,rgb);
	
	cv::Mat rmax=(rgb[0]/(rgb[1]+rgb[2])) > 1.0;
	cv::Mat dilated=dilate(rmax,5);
	cv::Mat labels=remove_smalls(dilated);
	
	rmax=labels;
	
	rmax=dilate(rmax,100);
	
	cv::Mat hsv;
	cv::cvtColor(img,hsv, CV_BGR2HSV);
	vector<cv::Mat> channels(3);
	cv::split(hsv,channels);

	rmax &= (channels[2] < 0.8);	
	
	rmax = remove_smalls(rmax);
	
	
	rmax.convertTo(rmax,CV_32F);
	cv::Mat row_counts=rmax*cv::Mat(rmax.cols,1,CV_32F,1.0f);
	cv::Mat column_counts=cv::Mat(1,rmax.rows,CV_32F,1.0f)*rmax;
	
 	float threshold=0.95f;
	pair<size_t,size_t> xbnds=bounds_gt(column_counts.ptr<float>(),column_counts.ptr<float>()+column_counts.cols,threshold);
	pair<size_t,size_t> ybnds=bounds_gt(row_counts.ptr<float>(),row_counts.ptr<float>()+row_counts.rows,threshold);
	
	circle circ;
 	circ.r=(xbnds.second-xbnds.first)/2.0;
	circ.x=xbnds.first+circ.r;
	circ.y=ybnds.first+circ.r;
	
	if(display)
	{
		display_floatmap(rmax);
		vector<cv::Mat> rgb(3);
		
		cv::split(img,rgb);
		for(int i=0;i<3;i++)
		{
			cv::threshold(rgb[i],rgb[i],1.0,1.0,CV_THRESH_TRUNC);
		}
		cv::merge(rgb,img);
		draw_circle(circ,img,cv::Scalar(0.0,1.0,0.0));
		display_floatmap(img);
	}
	return circ;
}

cv::Vec3d reflection_angle(double fovc_deg,const cv::Vec2d& normalized_circle_location,double ballrad=1.0)
{
	
	double r=ballrad;
	double f2_deg=fovc_deg/2;
	double f2=f2_deg*M_PI/180.0;
	double X=cv::norm(normalized_circle_location);
	double u=r/cos(f2);
	double m=tan(f2);
	double v=X*u;

	/*#x * m+v=y
#x^2+y^2 = r^2

#(y-v)/m=x
#(y^2-2yv+v^2)/m^2=x^2
#(y^2-2yv+v^2)/m^2=r^2-y^2
#y^2(1+1/m^2)-y(2v/m^2)+(v^2/m^2-r^2)=0
m2=m*m;
*/
	double m2=m*m;
	double a=1.0+1.0/m2;
	double b=-2*v/m2;
	double c=v*v/m2-r*r;

	//y=min(roots([a,b,c]))
	double y=(-b-sqrt(b*b-4*a*c))/(2.0*a); //a is always positive so +-,- is the right answer
	double x=sqrt(r*r-y*y);
	//x is the depth of the cylinder, y is the radius of the cylinder
	
	double px=x;
	double py=y;
	
	double alpha=atan2(py,px);
	double fovx=X*tan(f2);
	
	double rwidth=alpha+fovx;
	double reflect_angle=fovx+rwidth*2.0;
	
	y=r*sin(reflect_angle);
	x=r*cos(reflect_angle);
	
	cv::Vec2d xycoord=normalized_circle_location*(y/cv::norm(normalized_circle_location));
	return cv::Vec3d(xycoord[0],xycoord[1],x)/r;
}

cv::Vec3d get_lightsource_direction(const cv::Mat& image,float fovy,double ballrad,const circle& c)
{
	std::vector<cv::Mat> rgb(3);
	cv::split(image,rgb);
	
	std::vector<float> channel(image.rows*image.cols);
	static const int num_toppix=30;
	
	cv::Mat circbin(image.rows,image.cols,CV_8UC1,0);
	cv::circle(circbin,cv::Point2i(c.x,c.y),c.r,255,-1);
	
	cv::Vec2d dir2d(0.0f,0.0f);
	
	for(int i=0;i<3;i++)
	{
		copy(rgb[i].ptr<float>(),rgb[i].ptr<float>()+rgb[i].rows*rgb[i].cols,channel.begin());
		std::nth_element(channel.begin(),channel.begin()+num_toppix,channel.end());
		float threshold=channel[num_toppix];
		cv::Mat bin = rgb[i] > threshold & circbin;
		
		dir2d+=average_center(bin);
	}
	dir2d/=3.0;
	
	dir2d-=cv::Vec2d(c.x,c.y);
	dir2d/=c.r;
	
	double fovc=atan(2.0*c.r*tan(fovy)/((double)image.rows));
	
	return reflection_angle(fovc,dir2d);
}

std::vector<lightsource> get_lightsources(const std::vector<string>& images,const circle& c,float fovy,double ballrad=1.0,bool normalize_intensity=true)
{
	std::vector<lightsource> sources;
	for(int i=0;i<images.size();i++)
	{
		cv::Mat fm=read_floatmap(images[i]);
		lightsource ls;
		ls.intensity = median_color(fm);
		ls.direction = get_lightsource_direction(fm,fovy,ballrad,c);
	}
	if(normalize_intensity)
	{
		cv::Vec3f maxintensity(-10,-10,-10);
		for(int i=0;i<sources.size();i++)
		{
			for(int c=0;c<3;c++)
			{
				maxintensity[c]=std::max(sources[i].intensity[c],maxintensity[c]);
			}
		}
		for(int i=0;i<sources.size();i++)
		{
			for(int c=0;c<3;c++)
			{
				sources[i].intensity[c]/=maxintensity[c];
			}
		}
	}
	return sources;
}

double get_fovy(double focal_length=135,double sensor_width_y=24)
{
	return 2.0*atan(sensor_width_y/(2.0*focal_length));
}

int main(int argc,char** argv)
{
	try
	{
		vector<string> args(argv,argv+argc);
		vector<string> images;
		bool preview=true;
		double focal_length=-1.0;
		double effective_sensor=24.0;
		double ballrad=1.0;
		bool normalize_intensity=false;
		for(int i=1;i<args.size();i++)
		{
			if(args[i][0]=='-' && args[i][1]=='-')
			{
				std::string option=args[i].substr(2);
				if(option=="preview")
				{
					preview==true;
				}
				else if(option=="focal_length")
				{
					istringstream(args[++i]) >> focal_length;
				}
				else if(option=="effective_sensor")
				{
					istringstream(args[++i]) >> effective_sensor;
				}
				else if(option=="normalize_intensity")
				{
					normalize_intensity=true;
				}
			}
			else
			{
				images.push_back(args[i]);
			}
		}
		if(focal_length < 0.0)
		{
			throw std::invalid_argument("You MUST specify the focal_length of the camera with --focal_length");
		}
		double fovy=get_fovy(focal_length,effective_sensor);
		circle c=get_circle(read_floatmap(images[0]),preview);
		vector<string> directions(images.begin()+1,images.end());
		vector<lightsource> sources=get_lightsources(directions,c,fovy,ballrad,normalize_intensity);
		for(auto s: sources)
		{
			cout << s << endl;
		}
		
		return 0;
	} 
	catch(const std::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}
}
