#pragma once
#include <string>

struct cnf
{
	std::string hello_msg;
	bool is_keep_udp_conn;
	bool is_keep_udp_hole;
	bool start_info;
};

int cfgp(struct cnf conf);
