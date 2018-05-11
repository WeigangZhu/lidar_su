#ifndef LIDAR_H_
#define LIDAR_H_

#include<stdio.h>
#include<iostream>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <fcntl.h>
#include "rawdata.h"

int open_net();
void err_recvfrom(int );
void test_output(unpack_point_t*, FILE *fp1, FILE *fp2);


// 

int open_net()
{
	int sockfd;
    	struct sockaddr_in server;
    	

    	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		perror("Creatingsocket failed.");
		exit(1);
    	}

    	int opt=1;
    	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&opt,sizeof(opt)))
    	{
        	perror("setsockopt error!\n");
        	return -1;
    	}

    	bzero(&server, sizeof(server));
    	server.sin_family = AF_INET;
    	server.sin_port = htons(PORT);
    	server.sin_addr.s_addr = htonl(INADDR_ANY);

    	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) == -1) 
	{
		perror("Bind()error.");
		exit(1);
    	}
	return sockfd;

}

void err_recvfrom(int num)
{
	if (num < 0) 
		{
	    		perror("recvfrom() error\n");
	    		exit(1);
		}
		else if (num != PACKET_SIZE) 
		{
			std::cout << "incomplete rslidar packet read: " << num << " bytes";
		}
}


void test_output(unpack_point_t* data_unpack, FILE *fp1, FILE *fp2)
{


	for(int t=0 ; t<POINT_NUM; t++)	
	{
		fprintf(fp1,"%.8f\t%.8f\t%.8f\t%d\n", data_unpack->point[t].x,
						      data_unpack->point[t].y,
						      data_unpack->point[t].z,
						      data_unpack->point[t].intensity);
		if(data_unpack->point[t].intensity != 0)
			fprintf(fp2,"%.8f\t%.8f\t%.8f\t%d\n", data_unpack->point[t].x,
							      data_unpack->point[t].y,
							      data_unpack->point[t].z,
							      data_unpack->point[t].intensity);
	}
}



#endif
