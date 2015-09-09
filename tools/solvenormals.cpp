#include<cmath>
#include<opencv2/imgproc/imgproc.hpp>
#include"floatmaps.hpp"
#include"cilscanner.hpp"
#include<Eigen/Dense>
#include<Eigen/SVD>
using namespace std;

struct images_out
{
	cv::Mat normals;
	cv::Mat diffuse;
};
class Solver
{
public:
	const vector<lightsource>& lightsources;
	Eigen::Matrix<double,Eigen::Dynamic,3> A; 
	Eigen::JacobiSVD<Eigen::Matrix<double,Eigen::Dynamic,3> > svd;
	Solver(const vector<lightsource>& ls):
		lightsources(ls)
	{
		for(int i=0;i<3;i++)
		{
			for(int j=0;j<ls.size();j++)
			{
				A(j,i)=ls[j].direction[i];
			}
		}
		svd.compute(A,Eigen::ComputeThinU | Eigen::ComputeThinV);
	}
	cv::Vec4f solve_xyzd_normal(const Eigen::VectorXd& luminance)
	{
		Eigen::Vector3d solution=svd.solve(luminance);
		double diffuse=solution.norm();
		solution/=diffuse;
		return {solution[0],solution[1],solution[2],diffuse};
	}
};

struct final_result_out_per_pixel
{
	cv::Vec3f normal;
	cv::Vec3f diffuse;
	final_result_out_per_pixel():
		normal(0.0f,0.0f,0.0f),
		diffuse(0.0f,0.0f,0.0f)
	{}
};

images_out solve(const vector<int>& patterns,
		 const vector<lightsource>& lightsources,
		 const std::vector<cv::Mat>& inputs,
		 const cv::Mat& mask
		)
{
	images_out results;
	size_t rows=inputs[0].rows,cols=inputs[0].cols;
	results.normals=cv::Mat(rows,cols,CV_32FC3,0.0f);
	results.diffuse=cv::Mat(rows,cols,CV_32FC3,0.0f);
	
	size_t n=rows*cols;
	
	size_t p=patterns.size();
	vector<lightsource> newlightsources(p);
	for(int i=0;i<p;i++)
	{
		newlightsources[i]=lightsources[firstbitset(patterns[p])];
	}
	
	float* normalsout=(float*)results.normals.data;
	float* diffuseout=(float*)results.diffuse.data;
	
	Solver perpixelsolver(newlightsources);
	
	#pragma omp parallel for
	for(size_t i=0;i<n;i++)
	{
		Eigen::VectorXd l(p);
		final_result_out_per_pixel opp;
		if(mask.data[i])
		{
			for(int c=0;c<3;c++)
			{
				for(int j=0;j<p;j++)
				{
					const float* data=(const float*)inputs[j].data;
					l[j]=data[3*i+c]/newlightsources[j].intensity[c];
				}
				
				cv::Vec4f xyzd_c=perpixelsolver.solve_xyzd_normal(l);
				opp.normal+=cv::Vec3f(xyzd_c[0],xyzd_c[1],xyzd_c[2]);
				opp.normal/=3.0f;
				opp.diffuse[c]=xyzd_c[3];
			}
		}
		for(int c=0;c<3;c++)
		{
			normalsout[3*i+c]=opp.normal[c];
			diffuseout[3*i+c]=opp.diffuse[c];
		}
	}
	return results;
}



int main(int argc,char** argv)
{
	try
	{
		vector<string> args(argv,argv+argc);
		vector<string> images;
		std::string sequencefile="";
		std::string lightingfile="";
		std::string outputname="output";
		bool preview=false;
		
		for(int i=1;i<args.size();i++)
		{
			if(args[i][0]=='-' && args[i][1]=='-')
			{
				std::string option=args[i].substr(2);
				if(option=="preview")
				{
					preview==true;
				}
				else if(option=="sequencefile")
				{
					istringstream(args[++i]) >> sequencefile;
				}
				else if(option=="lightingfile")
				{
					istringstream(args[++i]) >> lightingfile;
				}
				else if(option=="outputname")
				{
					istringstream(args[++i]) >> outputname;
				}
			}
			else
			{
				images.push_back(args[i]);
			}
		}
		if(lightingfile=="")
		{
			throw std::invalid_argument("You MUST specify the lighting file with the calibration --lightingfile");
		}
		if(sequencefile=="")
		{
			throw std::invalid_argument("You MUST specify the sequence file with the pattern hex sequence with --sequencefile");
		}
		
		return 0;
	} 
	catch(const std::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}
}