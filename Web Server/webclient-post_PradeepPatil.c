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
#include <arpa/inet.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0) // to support memory functions bzeso
#define BUFFER_SIZE 2048
#define PATH_SIZE 1024
#define HOST_SIZE 1024

char* BuildReqString(int sockDisc , char ar1);

int main(int argc, char *argv[])

{
	printf("CMPE 207 HW1 webclient-post Bapugouda urf Pradeep Patil 011483277\n\n");
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

        struct addrinfo *AddrforLoop;
	struct addrinfo hints;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET ; // ipv4
	hints.ai_socktype = SOCK_STREAM;// TCP/IP

	char buff[BUFFER_SIZE]; //array to hold the received text from server
	struct addrinfo *servinfo;                                     // creating a addrinfo struct to hold o/p of getaddr info
	//Calling getaddrinfo
	int status = getaddrinfo(host,"http",&hints,&servinfo);  // resolving address

	/* Creat socket with valyes in Servinfo sonstruct*/
	//int sockfd = socket (servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);// v4 TCP/IP
	//int sockfd= socket(AF_INET,SOCK_STREAM,0);
        int sockfd;
	//Create the request string
	char* RequestString;
			int requestsize=100;
			requestsize+=strlen(argv[1]);
			RequestString=malloc(requestsize);
			// construction request line
			sprintf(RequestString,"%s %s HTTP/1.0\r\n\r\n","POST",argv[1]);


// creating sockets and Connecting to active address amoung all returned as servinfo by getaddrinfo
    for (AddrforLoop = servinfo; AddrforLoop != NULL; AddrforLoop = AddrforLoop->ai_next) {
               
	 sockfd = socket (AddrforLoop->ai_family,servinfo->ai_socktype,AddrforLoop->ai_protocol);// v4 TCP/IP
          // if not active     
	if (sockfd == -1) continue;
 	 
	 // connect to active server
               if (connect(sockfd, AddrforLoop->ai_addr, AddrforLoop->ai_addrlen) != -1)
                   break;                  

               close(sockfd);// close inactive/failed sockets
           }// for

	// no active servers
           if (AddrforLoop == NULL) {               
               fprintf(stderr, "Could not connect\n");
               exit(EXIT_FAILURE);
           }

			


	/* connect and Write */
   //if( connect(sockfd,servinfo->ai_addr,servinfo->ai_addrlen) == 0)
  // {  //printf("yes");

	   struct sockaddr_in  my_addr;
	   int myPort;char myIP[16];
	   char CrPort[6];
	   // Get my ip address and port
	   	bzero(&my_addr, sizeof(my_addr));
	   	int len = sizeof(my_addr);

	   	getsockname(sockfd, (struct sockaddr *) &my_addr, &len);
	   	inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
	   	myPort = ntohs(my_addr.sin_port);

	   	//printf("Local ip address: %s\n", myIP);
	   	//printf("Local port : %u\n", myPort);
			//construct request body
			strcat(RequestString,"ClientIP= ");
			strcat(RequestString,myIP);
			strcat(RequestString,"\r\n");
			strcat(RequestString,"ClientPort= ");
			sprintf(CrPort,"%u",myPort);			
			strcat(RequestString, CrPort);
			strcat(RequestString,"\r\n\r\n\0");
                       // printf("request is: %s\n", RequestString);
			


         // write the request string to the socket and call write shutdown.
	  int n = write(sockfd,RequestString,strlen(RequestString));
	  shutdown(sockfd, SHUT_WR);
  // }   
 //else printf("could not connect to host");
   
  int rd;
   //printf("response is :");
   while((rd= read(sockfd,buff,BUFFER_SIZE) )!=0 ){
	        if (rd==0)break;
	        buff[rd]='\0';
   	        printf("%s",buff);
   	        bzero(buff, BUFFER_SIZE);
         }

   //deallocationg resources
   //shutdown(sockfd, SHUT_RDWR);
	sleep(30);
   	close(sockfd);
   	freeaddrinfo(servinfo);
   	//printf("CMPE 207 HW1 webclient Bapugouda urf Pradeep Patil 277");
return 0;
}








