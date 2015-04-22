#include "cilscanner.hpp"
#include<iterator>
using namespace std;

ostream& operator<<(ostream& out,const lightsource& ls)
{
	for(int i=0;i<3;i++)
	{
		out << ls.direction[i] << ' ';
	}
	for(int i=0;i<3;i++)
	{
		out << ls.intensity[i] << ' ';
	}
	return out;
}
istream& operator>>(istream& in,lightsource& ls)
{
	for(int i=0;i<3;i++)
	{
		in >> ls.direction[i];
	}
	for(int i=0;i<3;i++)
	{
		in >> ls.intensity[i];
	}
	return in;
}

std::istream& operator>>(std::istream& in,patternsequence& ps)
{
	std::ios_base::fmtflags oldflags =in.setf(std::ios::hex,std::ios::basefield);
	ps=patternsequence((std::istream_iterator<lightpattern_t>(in)),std::istream_iterator<lightpattern_t>());
	in.setf(oldflags);
	return in;
}

static uint8_t popcnt(lightpattern_t v)
{
	v = v - ((v >> 1) & (lightpattern_t)~(lightpattern_t)0/3);                           // temp
	v = (v & (lightpattern_t)~(lightpattern_t)0/15*3) + ((v >> 2) & (lightpattern_t)~(lightpattern_t)0/15*3);      // temp
	v = (v + (v >> 4)) & (lightpattern_t)~(lightpattern_t)0/255*15;                      // temp
	return (lightpattern_t)(v * ((lightpattern_t)~(lightpattern_t)0/255)) >> (sizeof(lightpattern_t) - 1) * CHAR_BIT; // count	
}

bool isonelight(lightpattern_t lp)
{
	return popcnt(lp)==1;
}
int firstbitset(lightpattern_t lp)
{
	for(int i=0;i<sizeof(lightpattern_t)*8;i++)
	{
		if((lp & 0x1) == 1)
		{
			return i;
		}
	}
}




