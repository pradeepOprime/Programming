/*
 * udpchatsvr.c
 *
 *  Created on: Oct 22, 2018
 *      Author: pradeep207
 */



#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <error.h>


#include <pthread.h>

#include <errno.h>

extern int errno;
extern int errno;

//#include "rfm_pradeeppatil.h"

#define STDIN 0 //Fd for input



void printmargin(int sock,struct sockaddr_in Client_addr);

int main(int argc, char *argv[]){

	printf("CMPE 207 HW3 udpchat Bapugouda urf Pradeep Patil 011483277\n\n");
	/*port config*/
	char* PortChr="9191";
	if(argc>=2){PortChr=argv[1];}



	/*address declerations*/
	    struct addrinfo *AddrforLoop,*servinfo;
	  	struct addrinfo hints;
	  	memset(&hints, 0, sizeof (hints));
	  	hints.ai_family = AF_INET ; // ipv4
	  	hints.ai_socktype = SOCK_DGRAM;// UDP
	  	hints.ai_protocol = IPPROTO_UDP;//UDP

	  	struct sockaddr_in Addrforclient;
        socklen_t len;


	/*error handling veriable*/
	 	char GERROR[1024];
	  	bzero(GERROR,sizeof(GERROR));

	/*Call Get Addr Info*/
	  	int status = getaddrinfo(NULL,PortChr,&hints,&servinfo);
	  	  if(status<0){ strerror_r(errno,GERROR,sizeof(GERROR));printf("Error getinfo: %s",GERROR);
	  		  	        exit(errno);}


	/*Loop for binding*/

	  int SvrSock,z;
	/*create socket and bind*/
  for (AddrforLoop = servinfo; AddrforLoop != NULL; AddrforLoop = AddrforLoop->ai_next)
	 {	    SvrSock = socket (AddrforLoop->ai_family,servinfo->ai_socktype,AddrforLoop->ai_protocol);// v4
	  	  	if (SvrSock == -1) continue; // if not active
	  	    if ((z = bind(SvrSock, AddrforLoop->ai_addr, AddrforLoop->ai_addrlen)) != -1)
	  	    {/*printf("Bind done: %d\n",z);*/break;}  // connect to active server and break
	  	    close(SvrSock);// close inactive/failed sockets
	  	   }// for




  fd_set My_setRD;
  fd_set My_setWR;
 // FD_ZERO(&My_setRD);
  //FD_ZERO(&My_setWR);
  int Maxfds;
  Maxfds=SvrSock+10;
  char Buff[1024];
  char Buff2[1024];

  int ActivePort=0;
  int setter= 1;
  int ThisClientPort;

  struct sockaddr_in Addrforclientx;
          socklen_t lenx=sizeof(Addrforclientx);




while(1){
	FD_ZERO(&My_setRD);
    FD_SET(SvrSock,&My_setRD);
    FD_SET(STDIN,&My_setRD);


  int readyInd = select(Maxfds,&My_setRD,NULL,NULL,NULL);

  if (FD_ISSET(SvrSock,&My_setRD)){
	  bzero(Buff,1024);
	  //printf("Messege is \n ");
	  int n = recvfrom(SvrSock,Buff,1024,0,(struct sockaddr*)&Addrforclient,&len);



	  ThisClientPort = ntohs(Addrforclient.sin_port);

	  if (setter==1 && ThisClientPort>0){
		  ActivePort=ThisClientPort;
		  Addrforclientx = Addrforclient;
		  setter=0;}

	  if( ActivePort==ThisClientPort){
	  //printf("%d ",ThisClientPort);
      printmargin(SvrSock,Addrforclient);
	  printf("<%s> :", inet_ntoa(Addrforclient.sin_addr));
	  printf("%s \n",Buff);
	  FD_CLR(SvrSock,&My_setRD);
	 }
	 else{int z = sendto(SvrSock,"<<ServerBusy>>\n",strlen("<<ServerBusy>>\n"),0,(struct sockaddr*)&Addrforclient,len);
	 bzero(Buff,1024);
	 }

      }

  if (FD_ISSET(STDIN,&My_setRD)){
	  bzero(Buff2,1024);
	  fgets(Buff2,1024,stdin);
	  //strcat(Buff2,"hello from server");
	  int m = sendto(SvrSock,Buff2,strlen(Buff2),0,(struct sockaddr*)&Addrforclientx,lenx);
	  //printf("sent....\n");

	  FD_CLR(STDIN,&My_setRD);

      }


   }//While



	return 0;
}





void printmargin(int sock,struct sockaddr_in Client_addr)
{

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char strdata[100];
	//strftime(strdata,100,"%x-%I:%M%p",tm);
	strftime(strdata,100,"%c",&tm);
	printf("|%s| ",strdata);


	//struct sockaddr_in  Client_addr;
    int ClientPort;char ClientIP[16];
	char CrCPort[6];
		 // Get client ip address and port details, getpeername()
			   	bzero(&Client_addr, sizeof(Client_addr));
			   	int len = sizeof(Client_addr);

			   	//char ClientIP[16];
			   	getpeername(sock,(struct sockaddr *) &Client_addr, &len);
			  	inet_ntop(AF_INET, &Client_addr.sin_addr, ClientIP, sizeof(ClientIP));
			  	//char CrCPort[6];
			   	ClientPort = ntohs(Client_addr.sin_port);
			   	//printf("%s: ",ClientIP);



}




