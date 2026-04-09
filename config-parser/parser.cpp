#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cfgp.h>

int cfgp(struct cnf conf)
{
	std::ifstream file("config");
	if (file.is_open())
	{
		while (!file.eof())
		{
			std::string cl;
			std::string arg="";
			std::string pr="";

			getline(file,cl);
			int cpos=0;
			for (int i=0;cl[i]!='=';i++)
			{
				if (i>=cl.size()){break;}
				arg+=cl[i];
				cpos=i;
			}
			if (cpos+1<cl.size())
			{
				cpos+=1;
				for (cpos;cpos<cl.size();cpos++)
				{
					pr+=cl[cpos];
				}
			}
			if (arg=="hello-msg"){conf.hello_msg=pr;}
			if (arg=="keep-udp-conn"){if (pr=="true"){conf.is_keep_udp_conn=true;};if (pr=="false"){conf.is_keep_udp_conn=false;}}
			if (arg=="keep-udp-hole"){if (pr=="true"){conf.is_keep_udp_hole=true;};if (pr=="false"){conf.is_keep_udp_hole=false;}}
			if (arg=="start-info"){if (pr=="true"){conf.start_info=true;};if (pr=="false"){conf.start_info=false;}}
		}
	}
	else{return 1;}
	
	return 0;
}
