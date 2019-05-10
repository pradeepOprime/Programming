/*
 * mtrfmsvr.c
 *
 *  Created on: Oct 8, 2018
 *      Author: Pradeep
 */

#include<stdio.h>
#include<sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <pthread.h>

#include <errno.h>

extern int errno;
extern int errno;

#include "rfm_pradeeppatil.h"

int ThreadThisClient(int DClientSock);
void MySubString(char Result[],char InString[],int From,int End);
void CommandValiditation(char Opcode[],char Path[],char Incommand[]);
void ReadfileAtPath(char path[],char fileData[]);
void RemoveFileAtPath(char path[],char Message[]);
void MutexManup(int op);
void ConstructStatStr(char SString[]);

struct {
	pthread_mutex_t	MYmutex_G;
	unsigned int	ActiveThreads_G;
	unsigned int	CompletedThreads_G;
	unsigned int 	TotalCommands_G;

} ThreadStat;
//ThreadStat MYmutex_G ActiveThreads_G CompletedThreads_G TotalCommands_G


int main(int argc, char *argv[]){
printf("CMPE 207 HW3 mtrfmsvr Bapugouda urf Pradeep Patil 011483277\n\n");
  /*port config*/
  char* PortChr="9091";
  if(argc>=2){PortChr=argv[1];}

  /*address declerations*/
    struct addrinfo *AddrforLoop,*servinfo;
  	struct addrinfo hints;
  	memset(&hints, 0, sizeof (hints));
  	hints.ai_family = AF_INET ; // ipv4
  	hints.ai_socktype = SOCK_STREAM;// TCP/IP
	hints.ai_protocol = 0;          /* Any protocol */
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;

  	/*error handling veriable*/
  	char GERROR[1024];
  	bzero(GERROR,sizeof(GERROR));

  int status = getaddrinfo("localhost",PortChr,&hints,&servinfo);
  if(status<0){
	  strerror_r(errno,GERROR,sizeof(GERROR));printf("Error: %s",GERROR);
	  	        exit(errno);  }
  int SvrSock,z;
  /*create socket and bind*/
  for (AddrforLoop = servinfo; AddrforLoop != NULL; AddrforLoop = AddrforLoop->ai_next)
  {
    SvrSock = socket (AddrforLoop->ai_family,servinfo->ai_socktype,AddrforLoop->ai_protocol);// v4 TCP/IP
  	if (SvrSock == -1) continue; // if not active
    if ((z = bind(SvrSock, AddrforLoop->ai_addr, AddrforLoop->ai_addrlen)) != -1)
    {/*printf("Bind done: %d\n",z);*/break;}  // connect to active server and break
    close(SvrSock);// close inactive/failed sockets
   }// for


  /*listen starts*/
  int lis = listen(SvrSock,10);
  if(lis<0){
  	  strerror_r(errno,GERROR,sizeof(GERROR));printf("Error: %s",GERROR);
  	  	        exit(errno);
    }

  printf("Listning...\n");
  //sleep(30);
  int hops=0;
while(1){
  int AcceptSock=accept(SvrSock,(struct sockaddr *)&AddrforLoop, &AddrforLoop);
  if(AcceptSock<0){ strerror_r(errno,GERROR,sizeof(GERROR));printf("Error: %s",GERROR);
    	  	        exit(errno); }

  //printf("accepted: %d\n",AcceptSock);

  /*threading part*/
  pthread_t threadclient;
  pthread_attr_t	threadattr;
  (void) pthread_attr_init(&threadattr);
  (void) pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);

  int th = pthread_create(&threadclient, &threadattr, (void * (*)(void *))ThreadThisClient,(void *)((long)AcceptSock));
  if(th<0){ strerror_r(errno,GERROR,sizeof(GERROR));printf("Error: %s",GERROR);
      	  	        exit(errno); }
//hops++;
}
  //int th = ThreadThisClient(AcceptSock);

 // pthread_join(threadclient,NULL);

 /* printf("sleeping..\n");
  sleep(15);
  close(SvrSock);
  printf("\nclosed");
	return 0;*/
}

int ThreadThisClient(int ClientSock)
{


printf("thread %lu: Started\n",pthread_self());

MutexManup(1); // increment active threads

 //int loops=0;
 //int BuffSz=1024;
 char ReadBuffer[1024],WriteBuffer[1024];
 char ThreadStats[2048];
 char opcode[20],path[1024],RefinedCmd[1024],RmMessage[1024];
 char FileData[1024];
 int rd,wr;

 char GERRORF[1024];
 bzero(GERRORF,sizeof(GERRORF));

 while(1)
 {


   //printf("\nreading..\n");

   	  while(1){
	 rd=read(ClientSock,ReadBuffer,sizeof(ReadBuffer)-1);
	 if(rd<0){ strerror_r(errno,GERRORF,sizeof(GERRORF));printf("Error: %s",GERRORF);return 0;}
	 //printf("\ninreading..");
	 if((ReadBuffer[strlen(ReadBuffer)-1]==CmdEnd[1]) & (ReadBuffer[strlen(ReadBuffer)-2]==CmdEnd[0])) break;
   	  }
   	MutexManup(4);//increment recieved commands
	 //printf("Read from Client: %s\n",ReadBuffer);

	 MySubString(RefinedCmd,ReadBuffer,0,strlen(ReadBuffer)-3);
	// printf("Refined: %s\n",RefinedCmd);

	 CommandValiditation(opcode,path,RefinedCmd);
	 //printf("opcode is:%s:path is:%s:\n",opcode,path);
	 printf("thread %lu: %s\n",pthread_self(),opcode);

	 strcpy(WriteBuffer,RefinedCmd);

	 if(strcmp(opcode,"cat")==0){
	 ReadfileAtPath(path,FileData);

	 strcpy(WriteBuffer,FileData);
	 }

	 else if(strcmp(opcode,"rm")==0){

	 	RemoveFileAtPath(path,RmMessage);
	 	strcpy(WriteBuffer,RmMessage);
	 	 }

	 else if(strcmp(opcode,"stats")==0){
		 ConstructStatStr(ThreadStats);
	 	 strcpy(WriteBuffer,ThreadStats);
	 	 }

	 else if(strcmp(opcode,"exit")==0){
		 shutdown(ClientSock,SHUT_WR);
		 int er;
		 er=close(ClientSock);
		 if (er<0)printf("error closing socket");
		 MutexManup(2);//decrement active threads
		 MutexManup(3);//increment completed threads
		 printf("thread %lu: exited\n",pthread_self());
		 return 0;
	    }

	 else {
		 strcpy(WriteBuffer,"Invalid Command! try again");
	 	  }



     strcat(WriteBuffer,CmdEnd);

	 wr = write(ClientSock,WriteBuffer,sizeof(WriteBuffer));
	 if(wr<0){ strerror_r(errno,GERRORF,sizeof(GERRORF));printf("Error: %s",GERRORF);return 0;}

	 //printf("\n-------\n");
	 bzero(ReadBuffer,sizeof(ReadBuffer));

//loops++;
 }

 return 0;

 }


void MutexManup(int op){
	//ThreadStat MYmutex_G ActiveThreads_G CompletedThreads_G TotalCommands_G
	switch (op) {
	//increment active threads
		case	1:
			(void) pthread_mutex_lock(&ThreadStat.MYmutex_G);
			ThreadStat.ActiveThreads_G++;
			(void) pthread_mutex_unlock(&ThreadStat.MYmutex_G);
			break;

   //decrement active threads
		case	2:
			(void) pthread_mutex_lock(&ThreadStat.MYmutex_G);
			ThreadStat.ActiveThreads_G--;
			(void) pthread_mutex_unlock(&ThreadStat.MYmutex_G);
			break;
	//increment comlpeted connections
		case    3:
			(void) pthread_mutex_lock(&ThreadStat.MYmutex_G);
			ThreadStat.CompletedThreads_G++;
			(void) pthread_mutex_unlock(&ThreadStat.MYmutex_G);

			break;
	//increment total commands
		case    4:
			(void) pthread_mutex_lock(&ThreadStat.MYmutex_G);
			ThreadStat.TotalCommands_G++;
			(void) pthread_mutex_unlock(&ThreadStat.MYmutex_G);

		     break;
		default:
			printf("invalid operation");
		}



}

void ConstructStatStr(char SString[])
{
	//ThreadStat MYmutex_G ActiveThreads_G CompletedThreads_G TotalCommands_G
	(void) pthread_mutex_lock(&ThreadStat.MYmutex_G);

  sprintf(SString," Concurrent connections: %d\n Completed Connections : %d\n Total Overall Commands: %d\n"
		 , ThreadStat.ActiveThreads_G,ThreadStat.CompletedThreads_G, ThreadStat.TotalCommands_G );
  (void) pthread_mutex_unlock(&ThreadStat.MYmutex_G);

}


void RemoveFileAtPath(char path[],char Message[]){

	    char response[1024];
		bzero(response,sizeof(response));

		int x = remove(path);

	    if(x<0){
	    	strerror_r(errno,response,sizeof(response));
	        printf("Error in rm: %s\n",response);
	        strcpy(Message,response);
	    }
	    if(x==0){ printf("Deleted\n");
	    strcpy(Message,"Deleted");
	    }


}

void ReadfileAtPath(char path[],char fileData[]){

	     FILE *LFile;
		char FRBuff[1024];
		char FileHas[1024],FileERR[1024];
		bzero(FileHas,sizeof(FileHas));

		//command =
		LFile = fopen(path,"r");
		if(LFile == NULL)
			{
			int x=	strerror_r(errno,FileERR,sizeof(FileERR));
				printf("Error in cat: %s\n",FileERR);
				strcpy(fileData,FileERR);
			}
		else{
		while(fscanf(LFile," %[^\t\^]s",FRBuff)!= EOF)
		{
		  strcat(FileHas,FRBuff);
		  //printf(" 2: %s\n",FileHas);

		}//while
		}//else
		if(LFile != NULL)
		{strcpy(fileData,FileHas);
		//printf(" filedata: %s\n",fileData);
		fclose(LFile);
		}


}





void MySubString(char Result[],char InString[],int From,int End)
{	int c=0;
	while(c<=End){
		Result[c]=InString[From+c];	c++;
	}
	Result[c]='\0';
}


void CommandValiditation(char Opcode[],char Path[],char Incommand[]){

	/*cat*/
	if((Incommand[0]=='c' )& (Incommand[1]=='a') & (Incommand[2]=='t')){
		//printf("Its CAT\n");
		MySubString(Opcode,Incommand,0,2);
		MySubString(Path,Incommand,4,strlen(Incommand)-1);
	}

	else if((Incommand[0]=='r' )& (Incommand[1]=='m') ){
		//printf("Its rm\n");
				MySubString(Opcode,Incommand,0,1);
				MySubString(Path,Incommand,3,strlen(Incommand)-1);
	}

	else if((Incommand[0]=='s' ) & (Incommand[1]=='t')&(Incommand[2]=='a' ) & (Incommand[3]=='t') & (Incommand[4]=='s') ){

			//printf("Its stats\n");
					MySubString(Opcode,Incommand,0,4);
					MySubString(Path,"NONE",0,3);
		}

	else if((Incommand[0]=='e' ) & (Incommand[1]=='x')&(Incommand[2]=='i' ) & (Incommand[3]=='t') ){

				//printf("Its exit\n");
						MySubString(Opcode,Incommand,0,3);
						MySubString(Path,"NONE",0,3);
			}

	else {
		//printf("its invalid\n");
		strcpy(Opcode,"invalid command: ");
		//MySubString(Opcode,Incommand,0,2);
		//sprintf();
		strcat(Opcode,Incommand);
	    MySubString(Path,"NONE",0,3);
	     }

}

/*
 *  char GERROR[1024];
		bzero(GERROR,sizeof(GERROR));

		int x = remove(path);

	    if(x<0){
	    	strerror_r(errno,GERROR,sizeof(GERROR));
	        printf("Error: %s",GERROR);
	        //strcpy(Message,GERROR);
	    }
 */



