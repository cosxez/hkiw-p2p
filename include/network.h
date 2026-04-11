#pragma once
#include <string>

int gpb_ip(int sock,std::string *cipv4,unsigned short *cport);
int udp_conn(int sock,struct sockaddr_in addr);
void udp_read(int sock,struct sockaddr_in addr);
int send_file(int sock,struct sockaddr_in addr,std::string str);
void keep_udp_conn(int sock,struct sockaddr_in addr);
