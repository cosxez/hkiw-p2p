#include <iostream>
#include <network.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <vector>
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
		if (sendto(sock,&mka,sizeof(mka),0,(struct sockaddr*)&addr,sizeof(addr))<=0){std::cout<<"\nconnection broke\n>"<<std::flush;if (sock!=-1){close(sock);sock=-1;return;}}
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
			if (sock==-1){std::cout<<"\nconnection closed\n>"<<std::flush;return;}

			if (sb<=0){std::cout<<"\nconnection broke or interlocutor disconnected\n>"<<std::flush;close(sock);sock=-1;return;}
			else
			{
				if (faddr.sin_addr.s_addr==addr.sin_addr.s_addr && sock!=-1)
				{
					if (*(uint16_t*)buffer==0xbe3b || *(uint16_t*)buffer==0x3a1c){continue;}
					if (*(uint16_t*)buffer==0x3bad)
					{
						size_t fs;
						recvfrom(sock,&fs,sizeof(fs),0,(struct sockaddr*)&faddr,&ips);
						
						sb=recvfrom(sock,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&faddr,&ips);
						std::string str;
						for (int i=0;i<sb;i++){if ((i+2)<sb && buffer[i]=='.' && buffer[i+1]=='.' && buffer[i+2]=='/'){i+=2;continue;};str+=buffer[i];}
						if (!str.empty() && (str==".." || str=="../" || str[0]=='/')){str="file_from_p2p";}
						if (str.empty()){str="file_from_p2p";}
						if (access(str.c_str(),F_OK)==0){str=("ffp2p_"+str);}

						std::vector<unsigned char> fd(fs);
						unsigned int cpc=0;
						unsigned int lsr;
						while (cpc<fs)
						{
							lsr=recvfrom(sock,fd.data()+cpc,fd.size(),0,(struct sockaddr*)&faddr,&ips);
							if (lsr==2 && *(uint16_t*)(fd.data() + cpc)==0xe3dd){fd.erase(fd.begin()+cpc,fd.begin()+cpc+2);break;}
							cpc+=lsr;
						}

						std::ofstream file(str,std::ios::binary);
						if (file.is_open())
						{
							file.write(reinterpret_cast<const char*>(fd.data()),fs);
							file.close();
							std::cout<<"\nfile wrote \"" +str +"\"\n>"<<std::flush;
						}
						else{std::cout<<"\nerror: file dont open\n>"<<std::flush;}
						continue;
					}
					buffer[sb]='\0';
					std::cout<<"\ninterlocutor>";
					for (int i=0;i<sb;i++){std::cout<<buffer[i];}
					if (warn==false){std::cout<<"\n(you can also continue writing)\nyou>"<<std::flush;warn=true;}
					else{std::cout<<"\nyou>"<<std::flush;}
				}
			}
		}
		catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
	}
}

int send_file(int sock,struct sockaddr_in addr,std::string str)
{
	std::ifstream file(str,std::ios::binary);
	if (file.is_open())
	{
		file.seekg(0,std::ios::end);
		size_t fs=file.tellg();
		file.seekg(0,std::ios::beg);

		std::vector<unsigned char> fd(fs);
		file.read(reinterpret_cast<char*>(fd.data()),fs);
		file.close();

		unsigned short mff=0x3bad;
		sendto(sock,&mff,2,0,(struct sockaddr*)&addr,sizeof(addr));
		sendto(sock,&fs,sizeof(fs),0,(struct sockaddr*)&addr,sizeof(addr));
		sendto(sock,str.c_str(),str.size(),0,(struct sockaddr*)&addr,sizeof(addr));

		size_t cpc=0;
		while (cpc<fs)
		{
			if ((cpc+1024)<fd.size())
			{
				sendto(sock,&fd[cpc],1024,0,(struct sockaddr*)&addr,sizeof(addr));
				cpc+=1024;
			}
			else
			{
				sendto(sock,&fd[cpc],fd.size()-cpc,0,(struct sockaddr*)&addr,sizeof(addr));
				cpc+=fd.size()-cpc;
			}
		}
		unsigned short mgcn=0xe3dd;
		sendto(sock,&mgcn,2,0,(struct sockaddr*)&addr,sizeof(addr));
		std::cout<<"\nfile sended\n>"<<std::flush;
	}
	else{std::cout<<"\nerror: file dont exist\n>"<<std::flush;return 1;}
	
	return 0;
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
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}
