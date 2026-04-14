#include <iostream>
#include <network.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int gpb_ip(int sock,std::string *sipv4,unsigned short *cport)
{
	struct addrinfo hink;
	struct addrinfo *res;
	memset(&hink,0,sizeof(hink));

	hink.ai_family=AF_UNSPEC;
	hink.ai_socktype=SOCK_DGRAM;
	hink.ai_flags=AI_PASSIVE;
	hink.ai_protocol=0;
	hink.ai_canonname=NULL;
	hink.ai_addr=NULL;
	hink.ai_next=NULL;

	if (getaddrinfo("stun.rtc.yandex.net","3478",&hink,&res)!=0){std::cout<<"failed get public ip\n";return 1;}

	unsigned char req[20];memset(&req,0,20);
	*(uint16_t*)(&req[0])=htons(0x0001);
	*(uint16_t*)(&req[2])=htons(0x0000);
	*(uint32_t*)(&req[4])=htonl(0x2112A442);

	unsigned char buff[512];
	struct sockaddr_in addr;
	sendto(sock,req,20,0,res->ai_addr,res->ai_addrlen);
	freeaddrinfo(res);

	socklen_t addr_len=sizeof(addr);
	int sb=recvfrom(sock,buff,sizeof(buff),0,(struct sockaddr*)&addr,&addr_len);
	if (sb>=32)
	{
		*sipv4=(std::to_string(buff[28] ^ 0x21) + '.' + std::to_string(buff[29]^0x12) + '.' + std::to_string(buff[30]^0xA4) + '.' + std::to_string(buff[31]^0x42));
		
		unsigned short p=(buff[26]<<8) | (buff[27]);
		*cport=p ^ 0x2112;
	}
	return 0;
}
