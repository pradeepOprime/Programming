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
#include <arpa/inet.h>
#include <sys/select.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)// to support memory functions bzeso
#define BUFFER_SIZE 2048
#define PATH_SIZE 1024
#define HOST_SIZE 1024
#define STDIN 0

void printmargin(int sock);

int main(int argc, char *argv[])

{
	printf("CMPE 207 HW4 udpchatclient Bapugouda urf Pradeep Patil 277\n");
	if(argc < 3){printf("Usage: udpclient host service <message>"); exit(1);}
	//printf("%s and %s",host,path);
	// time veriable declerations
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char host[20] ;
     strcpy( host,argv[1]);


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
        socklen_t len;
        len= sizeof(servinfo);
   // printf("info gathered\n");

    /* Creating socket with valyes in Servinfo sonstruct*/
	/*integer sockfd holds the which holds socket descriptor*/
	int sockfd = socket (servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);// V4 UDP
	//printf("socket done");
	//int sockfd= socket(AF_INET,SOCK_STREAM,0);

	//printf("79\n");
	char Bufk[BUFFER_SIZE];
	bzero(Bufk,BUFFER_SIZE);

  fd_set My_setRD;
 
  int Maxfds;
  Maxfds=sockfd+10;
  char Buff[1024];
  char Buff2[1024];

while(1){
    FD_ZERO(&My_setRD);
    FD_SET(sockfd,&My_setRD);
    FD_SET(STDIN,&My_setRD);


  int readyInd = select(Maxfds,&My_setRD,NULL,NULL,NULL);

  
 if (FD_ISSET(STDIN,&My_setRD)){
	  bzero(Buff2,1024);
	  fgets(Buff2,1024,stdin);
	  
	int m = sendto(sockfd,Buff2,sizeof(Buff2),0,servinfo->ai_addr,servinfo->ai_addrlen);
	  //printf("sent to server....\n");
	  FD_CLR(STDIN,&My_setRD);
      }
	
 if (FD_ISSET(sockfd,&My_setRD)){
	  bzero(Buff,1024);
	  //printf("server says \n: ");
  	printmargin(sockfd);
	printf("<%s> :",host);
	
     int j = recvfrom(sockfd,Buff,BUFFER_SIZE,0,servinfo->ai_addr,&servinfo->ai_addrlen);
         if (Buff[strlen(Buff)-2]=='>' || Buff[strlen(Buff)-3]=='>' ){
          printf("%s \n",Buff);
		exit(0);
          }
          
	

	  printf("%s \n",Buff);
	  FD_CLR(sockfd,&My_setRD);

      }

	

  }//while



//printf("Client's Current Time: %d %d %d %d:%d:%d PDT\n",tm.tm_mday,tm.tm_mon + 1, tm.tm_year + 1900,   tm.tm_hour, tm.tm_min, tm.tm_sec);

//			printf("length (%ld): %s\n",strlen(Bufk), Bufk);

   //deallocationg resources
    	close(sockfd);
    	freeaddrinfo(servinfo);
   	//printf("CMPE 207 HW1 webclient Bapugouda urf Pradeep Patil 277");
return 0;
}



void printmargin(int sock)
{

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char strdata[100];
	//strftime(strdata,100,"%x-%I:%M%p",tm);
	strftime(strdata,100,"%c",&tm);
	printf("|%s| ",strdata);


	



}










