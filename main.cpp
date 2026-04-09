#include <iostream>
#include <network.h>
#include <cfgp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <chrono>

int main()
{
	int sock=socket(AF_INET,SOCK_DGRAM,0);
	std::string ip="";
	unsigned short port=0;
	
	const char* command_list[]={"help - commands information","conn - connect to","inf-cs - information about current session","clear - clear screen","exit - exit from program", "inp - show base information","sreq - stun request"};
	
	if (gpb_ip(sock,&ip,&port)==0)
	{
		struct cnf conf;
		if (cfgp(conf)!=0)
		{
			conf.hello_msg="hkiw.p2p";
			conf.is_keep_udp_conn=true;
			conf.is_keep_udp_hole=false;
			conf.start_info=true;
		}

		if (!conf.hello_msg.empty()){std::cout<<conf.hello_msg<<std::endl;}
		if (conf.start_info==true){for (unsigned short i=0;i<sizeof(command_list)/8;i++){for (int j=0;j<command_list[i][j]!='\0';j++){std::cout<<command_list[i][j];};std::cout<<'\n';};std::cout<<'\n';}
		
		std::cout<<"your public ip and port: "<<ip<<' '<<port<<'\n';
		while (1)
		{
			std::string cmd;
			while (cmd.empty()){std::cout<<'>';getline(std::cin,cmd);}
			if (cmd=="clear"){std::cout<<"\033c";}
			if (cmd=="exit"){break;}
			if (cmd=="inf-cs"){std::string restr="Keep alive udp connection is ";restr+=(conf.is_keep_udp_conn ? "true" : "false"); restr+="\nKeep alive udp hole is ";restr+=(conf.is_keep_udp_hole ? "true" : "false");restr+='\n';std::cout<<restr;}
			if (cmd=="inp"){for (unsigned short i=0;i<sizeof(command_list)/8;i++){for (int j=0;j<command_list[i][j]!='\0';j++){std::cout<<command_list[i][j];};std::cout<<'\n';};std::cout<<"your public ip and port: "<<ip<<' '<<port<<'\n';}
			if (cmd=="sreq"){ip="";port=0;gpb_ip(sock,&ip,&port);std::cout<<"your public ip and port: "<<ip<<' '<<port<<'\n';}
			if (cmd=="conn")
			{
				std::cout<<"Enter interlocutor ip: ";
				std::string i_ip;
				while (i_ip.empty()){std::cout<<"ip>";getline(std::cin,i_ip);}
				std::cout<<"Enter interlocutor port: ";
				std::string i_p;
				while (i_p.empty()){std::cout<<"port>";getline(std::cin,i_p);}

				try
				{
					struct sockaddr_in addr;
					addr.sin_family=AF_INET;
					addr.sin_port=htons(port);
					addr.sin_addr.s_addr=INADDR_ANY;

					bind(sock,(struct sockaddr*)&addr,sizeof(addr));
					
					struct sockaddr_in faddr;
					faddr.sin_family=AF_INET;
					faddr.sin_port=htons(std::stoi(i_p));

					inet_pton(AF_INET,i_ip.c_str(),&faddr.sin_addr);

					if (udp_conn(sock,faddr)==0)
					{
						std::cout<<"Enter udpclose for exit\n";
						if (conf.is_keep_udp_conn==true){std::thread(keep_udp_conn,sock,faddr).detach();}
						std::thread(udp_read,sock,faddr).detach();
						while (1)
						{
							std::string ccmd;
							while (ccmd.empty()){std::cout<<"you>";getline(std::cin,ccmd);}
							if (ccmd=="udpclose"){close(sock);break;}
							sendto(sock,ccmd.c_str(),ccmd.size(),0,(struct sockaddr*)&faddr,sizeof(faddr));
						}
					}
				}
				catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
			}
		}
		close(sock);
	}
	return 0;
}
