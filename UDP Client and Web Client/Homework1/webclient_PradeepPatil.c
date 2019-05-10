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

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0) // to support memory functions bzeso
#define BUFFER_SIZE 2048
#define PATH_SIZE 1024
#define HOST_SIZE 1024

int main(int argc, char *argv[])

{
	printf("CMPE 207 HW1 webclient Bapugouda urf Pradeep Patil 277\n");
	if(argc < 2){printf("Usage: webclient url"); exit(1);}


// Seperating host and path
	int success = 0;
	//char url[2048]= argv[1];
	char path[PATH_SIZE];
	char host[HOST_SIZE];
	if (sscanf(argv[1], "http://%99[^/]/%199[^\n]", host, path) == 2) {
	success = 1; /* http://hostname/page */
	}
	else if (sscanf(argv[1], "http://%99[^/]/[^\n]", host) == 1) {
	success = 1; /* http://hostname/ */
	}
	else if (sscanf(argv[1], "http://%99[^\n]", host) == 1) {
	success = 1; /* http://hostname */
	}

	//printf("%s and %s",host,path);


	struct addrinfo hints;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET ; // ipv4
	hints.ai_socktype = SOCK_STREAM;// TCP/IP

	char buff[BUFFER_SIZE]; //array to hold the received text from server

	    /*struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(80);
		addr.sin_addr.s_addr = 0x0f02000a;*/

	struct addrinfo *servinfo;                                     // creating a addrinfo struct to hold o/p of getaddr info
	//Calling getaddrinfo
	int status = getaddrinfo(host,"80",&hints,&servinfo);  // resolving address

	/* Creat socket with valyes in Servinfo sonstruct*/
	int sockfd = socket (servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);// v4 TCP/IP
	//int sockfd= socket(AF_INET,SOCK_STREAM,0);

	//Create the request string
	char* RequestString;
	int requestsize=22;
	requestsize+=strlen(argv[1]);
	RequestString=malloc(requestsize);
	sprintf(RequestString,"%s %s HTTP/1.0\r\n","GET",argv[1]);
	strcat(RequestString,"\r\n");

	//printf("REQ is: %s",RequestString );
	//char Hardreq[]="GET http://localhost/ HTTP/1.0\r\n\r\n";
   //printf("RHED is: %s",Hardreq );

	/* connect and Write */
   if( connect(sockfd,servinfo->ai_addr,servinfo->ai_addrlen) == 0)
   {  //printf("yes");
	  int n = write(sockfd,RequestString,strlen(RequestString));
	  shutdown(sockfd, SHUT_RDWR);
   }
   else printf("could not connect to host");

   while(read (sockfd,buff,BUFFER_SIZE-1) !=0 ){
   	        printf("%s",buff);
   	        bzero(buff, BUFFER_SIZE);
         }

   //deallocationg resources
   //shutdown(sockfd, SHUT_RDWR);
   	close(sockfd);
   	freeaddrinfo(servinfo);
   	//printf("CMPE 207 HW1 webclient Bapugouda urf Pradeep Patil 277");
return 0;
}


