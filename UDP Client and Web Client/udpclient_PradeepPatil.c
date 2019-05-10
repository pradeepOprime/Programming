/*
 * udpclient.c *
 *  Created on: Sep 19, 2018
 *      Author: pradeep 011483277
 */

#define _POSIX_C_SOURCE 200112L //to support address info
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)// to support memory functions bzeso
#define BUFFER_SIZE 2048
#define PATH_SIZE 1024
#define HOST_SIZE 1024

int main(int argc, char *argv[])

{
	printf("CMPE 207 HW1 udpclient Bapugouda urf Pradeep Patil 277\n");
	if(argc < 4){printf("Usage: udpclient host service <message>"); exit(1);}
	//printf("%s and %s",host,path);
	// time veriable declerations
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

  // struct to hold host address details setting IPV4 , and UDP Datagram
	struct addrinfo hints;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET ; // ipv4
	hints.ai_socktype = SOCK_DGRAM;// UDP
	hints.ai_protocol = IPPROTO_UDP;//UDP
	char buff[BUFFER_SIZE]; //array to hold the received text from server

	// creating a addrinfo struct to hold o/p of getaddrinfo(3)//Calling getaddrinfo
	struct addrinfo *servinfo;
	int status = getaddrinfo(argv[1],argv[2],&hints,&servinfo);  // resolving address
   // printf("info gathered\n");

    /* Creating socket with valyes in Servinfo sonstruct*/
	/*integer sockfd holds the which holds socket descriptor*/
	int sockfd = socket (servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);// V4 UDP
	//printf("socket done");
	//int sockfd= socket(AF_INET,SOCK_STREAM,0);

	//printf("79\n");
			char Bufk[BUFFER_SIZE];
			bzero(Bufk,BUFFER_SIZE);


		/*Sending the datagram on socket sockfd to host details present in Servinfo*/
		int k = sendto(sockfd,argv[3],strlen(argv[3]),0,servinfo->ai_addr,servinfo->ai_addrlen);
			if (k<0)printf("Error: Cant send");

			//printf("sendto done\n");

	    /*Receiving udp datagram from host at socketfd */
		int	 j = recvfrom(sockfd,Bufk,BUFFER_SIZE,0,servinfo->ai_addr,&servinfo->ai_addrlen);
					if (j<0)printf("Error: Cant read");

					//printf("recv done\n");





printf("Client's Current Time: %d %d %d %d:%d:%d PDT\n",tm.tm_mday,tm.tm_mon + 1, tm.tm_year + 1900,   tm.tm_hour, tm.tm_min, tm.tm_sec);

			printf("length (%ld): %s\n",strlen(Bufk), Bufk);

   //deallocationg resources
    	close(sockfd);
    	freeaddrinfo(servinfo);
   	//printf("CMPE 207 HW1 webclient Bapugouda urf Pradeep Patil 277");
return 0;
}




