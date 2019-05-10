/*
 * udpclient.c *
 *  Created on: Sep 19, 2018
 *      Author: pradeep 011483277
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "rfm_pradeeppatil.h"



int main(int argc, char *argv[])

{
	printf("CMPE 207 HW3 rfmclient Bapugouda urf Pradeep Patil 277\n");
	if(argc < 2){printf("Usage: webclient url"); exit(1);}
	/*port config*/
        char* PortChr="9091";
	char* Hostname=argv[1];
        if(argc>=3){PortChr=argv[2];}


	/*address struct declerations*/
        struct addrinfo *AddrforLoop;
	struct addrinfo hints,ConctAddr;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET ; // ipv4
	hints.ai_socktype = SOCK_STREAM;// TCP/IP

	
	struct addrinfo *servinfo;                                     // creating a addrinfo struct to hold o/p of getaddr info
	//Calling getaddrinfo
	int status = getaddrinfo(Hostname,PortChr,&hints,&servinfo);  // resolving address

	/* Creat socket with valyes in Servinfo sonstruct*/
	//int sockfd = socket (servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);// v4 TCP/IP
	//int sockfd= socket(AF_INET,SOCK_STREAM,0);
        int sockfd;
	
// creating sockets and Connecting to active address amoung all returned as servinfo by getaddrinfo
    for (AddrforLoop = servinfo; AddrforLoop != NULL; AddrforLoop = AddrforLoop->ai_next) 
      {
          sockfd = socket (AddrforLoop->ai_family,servinfo->ai_socktype,AddrforLoop->ai_protocol);// v4 TCP/IP
               
	if (sockfd == -1) continue;// if not active
 	 
	 if (connect(sockfd, AddrforLoop->ai_addr, AddrforLoop->ai_addrlen) != -1)
             {printf("connected..");break;}// connect to active server                  

         close(sockfd);// close inactive/failed sockets
       }// for

	// no active servers
           if (AddrforLoop == NULL) {fprintf(stderr, "Could not connect\n");exit(EXIT_FAILURE);}

    /*communication with the server*/
     char command[20];
     int BuffSz=1024;
     char ReadBuf[1024];
     int loops=0,rd;
    
     while(1)
    { 
       // printf("entring while..\n");
	printf("\n\ncommand> ");
        scanf(" %[^\t\n]s",command);
	strcat(command,CmdEnd);
       //printf("\ncommand is: %s",command);
      
      int wrt = write(sockfd,command,strlen(command));
      //printf("\nwrote\n");
	if((command[0]=='e')&(command[1]=='x')&(command[2]=='i')&(command[3]=='t'))
         {
          shutdown(sockfd,SHUT_WR);
	  close(sockfd);
	  return 0;
         }
    
    //printf("\nwrote%s\n",command);
    //printf("reading..\n");
   while(1)
   {
    rd= read(sockfd,ReadBuf,sizeof(ReadBuf)); 
       //printf("Inreading..:%s\n",ReadBuf);
   if((ReadBuf[strlen(ReadBuf)-1]==CmdEnd[1]) & (ReadBuf[strlen(ReadBuf)-2]==CmdEnd[0]))
      {
       ReadBuf[strlen(ReadBuf)-2]='\0';
       break; 
      } 

    }//while
    //printf("broke:\n");
    printf("%s",ReadBuf);
    bzero(ReadBuf,sizeof(ReadBuf));
    //loops++;
    }//loops while



			
   // sleep(30);
    close(sockfd);


return 0;
}








