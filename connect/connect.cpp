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
		ssize_t sb=recvfrom(sock,&chkc,sizeof(chkc),0,(struct sockaddr*)&addr,&ips);
		if (chkc==0xbe3b){*is_conn=true;std::cout<<"\nconnected\n";break;return;}
	}
}

void keep_udp_conn(int sock,struct sockaddr_in addr)
{
	unsigned short mka=0x3a1c;
	while (1)
	{
		if (sendto(sock,&mka,sizeof(mka),0,(struct sockaddr*)&addr,sizeof(addr))<=0){std::cout<<"\nconnection broke\n";if (sock!=-1){close(sock);sock=-1;return;}}
		std::this_thread::sleep_for(std::chrono::seconds(30));
	}
}

void udp_read(int sock,struct sockaddr_in addr)
{
	unsigned char buffer[2048];
	struct sockaddr_in faddr;

	socklen_t ips=sizeof(faddr);

	bool warn=false;
	while (1)
	{
		try
		{
			ssize_t sb=recvfrom(sock,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&faddr,&ips);
			if (sock==-1){std::cout<<"\nconnection closed\n";return;}

			if (sb<=0){std::cout<<"\nconnection broke or interlocutor disconnected\n";close(sock);sock=-1;return;}
			else
			{
				if (faddr.sin_addr.s_addr==addr.sin_addr.s_addr && sock!=-1)
				{
					if (*(uint16_t*)buffer==0x3a1c){continue;}
					buffer[sb]='\0';
					std::cout<<"\ninterlocutor>";
					for (int i=0;i<sb;i++){std::cout<<buffer[i];}
					if (warn==false){std::cout<<"\n(you can countinue writing)\nyou>"<<std::flush;warn=true;}
					else{std::cout<<"\nyou>"<<std::flush;}
				}
			}
		}
		catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
	}
}


int udp_conn(int sock,struct sockaddr_in addr)
{
	bool is_conn=false;
	std::thread(udp_listen_conn,sock,addr,&is_conn).detach();
	unsigned short mgn_c=0xbe3b;
	std::cout<<"connection..."<<std::flush;
	while (1)
	{
		sendto(sock,&mgn_c,sizeof(mgn_c),0,(struct sockaddr*)&addr,sizeof(addr));
		if (is_conn==true){break;}
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	return 0;
}
