#pragma once
#include <string>

int gpb_ip(int sock,std::string *cipv4,unsigned short *cport);
int udp_conn(const char* ip_addr,unsigned short port,int sock);
