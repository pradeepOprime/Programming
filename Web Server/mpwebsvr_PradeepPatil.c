/*
 * Server.c
 *
 *  Created on: Sep 26, 2018
 *      Author: pradeep
 */



#include<stdio.h>
#include<sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 1024

int HandleClientatSock(int SockID);

int main()
{
    int opt=1, ProcessID;
	struct sockaddr_in AddressHold;
	AddressHold.sin_family=AF_INET; // IPV4
	AddressHold.sin_addr.s_addr = INADDR_ANY; // local
	AddressHold.sin_port=htons(80);
	socklen_t AddrLen=sizeof(AddressHold);

  printf("CMPE 207 HW2 mpwebsvr Pradeep Patil 011483277 \n");

	//printf("Port is : %d \n",AddressHold.sin_port);

	int sockfd = socket(AF_INET,SOCK_STREAM,0);

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt, sizeof(opt));

	int j = bind(sockfd,(struct sockaddr *)&AddressHold,sizeof(AddressHold));
    //int j = bind(sockfd,&AddressHold.sin_addr.s_addr,sizeof(AddressHold.sin_addr.s_addr));

    int k = listen(sockfd,10);
    printf("listning...\n");



    int loop =0;
    while(1){

    	int NewAcceptSock = accept(sockfd,(struct sockaddr *)&AddressHold, &AddrLen);
        //int NewAcceptSock = accept(sockfd,AddressHold.sin_addr.s_addr,sizeof(AddressHold));
    	//printf("Sock is: %d\n",NewAcceptSock);

    	ProcessID = fork();

    	if(ProcessID<0){printf("Fork error");exit(1);}

    	if(ProcessID==0){
    	close(sockfd);
    	HandleClientatSock(NewAcceptSock);
    	exit(0);
    	}

    	else if(ProcessID>0){close(NewAcceptSock);}



   // loop++;
    }
    sleep(5);
    //deallocation
    close(sockfd);
    //freeaddrinfo(AddressHold);
return 0;
}




int HandleClientatSock(int SockID){
printf("-----------\n");

int CPID= getpid();
int PPID= getppid();

	           struct sockaddr_in  Client_addr;
		   int ClientPort;char ClientIP[16];
		   char CrCPort[6];
	 // Get client ip address and port details, getpeername()
		   	bzero(&Client_addr, sizeof(Client_addr));
		   	int len = sizeof(Client_addr);

		   	getpeername(SockID, (struct sockaddr *) &Client_addr, &len);
		   	inet_ntop(AF_INET, &Client_addr.sin_addr, ClientIP, sizeof(ClientIP));
		   	ClientPort = ntohs(Client_addr.sin_port);

		   	//printf("Client ip address: %s   ", ClientIP);
		   	printf("Server-Pid: %d , Child-Pid: %d , Client-IP: %s Client-Port : %u\n\n",PPID,CPID, ClientIP,ClientPort);

			sprintf(CrCPort,"%u",ClientPort);//int to char conversion
			//printf("%s",CrCPort);

	   // printf("In Function\n");
	   // printf("read start...\n");
	   // printf("Sock in finction is: %d \n",SockID);
	    
	    char Buffer[BUFFER_SIZE];
	    char* ReadData;//[BUFFER_SIZE];
	    ReadData=malloc(BUFFER_SIZE);
	
	// Read the client request. 
	  printf("Clint request is: \n");
	while (1){ 
	    int rd;
	    rd = read(SockID,Buffer,sizeof(Buffer)-1);
	    if (rd==0)break;
	    strcat(ReadData,Buffer);
	    Buffer[rd]='\0';
	    printf("%s",Buffer);
	    //bzero(Buffer,sizeof(Buffer));
	   }
    //adding sudo processing delay
    //sleep(10);
     char RSLine[]="HTTP/1.0 200 OK\r\n";
     char RSHeaders[100];
     char DocTop[]="<!DOCTYPE html>\n<html><body><blockquote>\n";
     char DocBot[]="</blockquote></body></html>\n";
     char ResponseString[BUFFER_SIZE];
     sprintf(RSHeaders,"ClientIP: %s \nClientPort: %s \n\n\r",ClientIP,CrCPort);
    sprintf(ResponseString,"%s%s%s%s%s",RSLine,RSHeaders,DocTop,ReadData,DocBot);

	   //printf("\n read: %s\n",ReadData);
	    write(SockID,ResponseString,strlen(ResponseString));
            shutdown(SockID,SHUT_WR);
	    sleep(150);

	// deallocating resource by closing socket
	    close(SockID);
        //printf("done with client\n\n");
//printf("-----------\n");

	return 0;
}





