#pragma once
#include <string>

struct cnf
{
	std::string hello_msg;
	bool is_keep_udp_conn;
	bool is_keep_udp_hole;
	bool start_info;
	bool auto_gpbip;
};

int cfgp(struct cnf *conf);
