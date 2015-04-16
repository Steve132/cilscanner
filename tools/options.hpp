#ifndef OPTIONS_H
#define OPTIONS_H

#include<deque>
#include<sstream>

class options
{
protected:
	std::deque<std::string> ops;
	std::ostream& hout;
	const char* summary;
	const char* description;
	size_t positionals;
	const bool has_help;

	template<class ArgType>
	ArgType poor_lexical_cast(const std::string& s) const
	{
		ArgType a;
		std::istringstream(s) >> a;
		return a;
	}
	bool help_detect()
	{
		size_t i;
		for(i=0;i<ops.size();i++)
		{
			if(ops[i]=="-h" || ops[i]=="--help")
			{
				return true;
			}
		}
	}

	void print_arghelp(const std::string& name,const std::vector<std::string>& triggers,const std::string& type,const std::string& description,bool RO)
	{
		ho << "\t";
		for(auto t:triggers)
		{
			ho << "\t" << t << "\n";
		}
		ho << "\t" << name << "(" << type << ")" << (RO ? "\tREQUIRED\t" : "\tOPTIONAL\t") << description << "\n";
	}
	template<class ArgType>
	ArgType do_help(const std::string& name,const std::vector<std::string>& triggers,const std::string& type,const std::string& description)
	{
		print_arghelp(name,triggers,type,description,true);
		return ArgType();
	}
	template<class ArgType>
	ArgType do_help(const std::string& name,const std::vector<std::string>& triggers,const std::string& type,const std::string& description,const ArgType& def)
	{
		print_arghelp(name,triggers,type,description,false);
		return def;
	}
	
	template<class ArgType>
	
public:
	template<class Iter>
	options(Iter b,Iter e,const char* s="No summary",const char* d="No program description",std::ostream& ho=std::cout):
		ops(b,e),
		summary(s),
		description(d),
		has_infinite_positional(false),
		has_help(help_detect()),
		hout(ho),
		positionals(0)
	{
		
	}

	template<class ArgType>
	ArgType named_argument(const std::string& name,const std::vector<std::string>& triggers,const std::string& type,const std::string& description)
	{
		if(has_help)
		{
			return do_help<ArgType>(name,type,description);
		}
		for(int i=0;i<triggers;i++)
		{
			
		}
	}

	template<class ArgType>
	ArgType positional_argument(const std::string& name,const std::string& type,const std::string& description)
	{
		if(has_help)
		{
			return do_help<ArgType>(name,type,description);
		}
	}
};

#endif
