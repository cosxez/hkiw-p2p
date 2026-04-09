#include <iostream>
#include <network.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>

void udp_listen_conn(int sock,struct sockaddr_in addr,bool *is_conn)
{
	socklen_t ips=sizeof(addr);
	while (1)
	{
		unsigned short chkc;
		int sb=recvfrom(sock,&chkc,sizeof(chkc),0,(struct sockaddr*)&addr,&ips);
		if (chkc==0xb3b){*is_conn=true;std::cout<<"connected\n";return;}
	}
}

int udp_conn(const char* ip_addr,unsigned short port,int sock)
{
	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	
	inet_pton(AF_INET,ip_addr,&addr.sin_addr);

	bool is_conn=false;
	std::thread(udp_listen_conn,sock,addr,&is_conn).detach();
	unsigned short mgn_c=0xbe3b;
	while (1)
	{
		sendto(sock,&mgn_c,sizeof(mgn_c),0,(struct sockaddr*)&addr,sizeof(addr));
		if (is_conn==true){break;}
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	return 0;
}
