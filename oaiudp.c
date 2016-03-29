#include <sys/types.h>

#include <sys/socket.h>

#include <string.h>

#include <netinet/in.h>

#include <stdio.h>

#include <stdlib.h>

#include <pthread.h>

#include <unistd.h>


#define OAI_PORT 8888

#define MAXLINE 1024

#define DBS_NUM 10


void *servthread(void *arg);

void *clithread(void *arg);

int oaisockfd = 0;

struct sockaddr_in oaiservaddr;

pthread_t oaiudptid;

int bsstart = 0;  //state flag to control the start of the basestation;

struct {
	int dbsopen;  //dbs state, 0 stands for close, 1 stands for open. only state 1 can provide service;
	struct sockaddr_in dbssockaddr;
}dbsstat[DBS_NUM];

int BSid = -1;

int init_udpserv(void)

{

int temp;

bsstart = 0;

oaisockfd = socket(AF_INET, SOCK_DGRAM, 0); /* create a socket */

/* init servaddr */

bzero(&oaiservaddr, sizeof(oaiservaddr));
bzero(&dbsstat[0],sizeof(dbsstat[0])*DBS_NUM);

oaiservaddr.sin_family = AF_INET;

oaiservaddr.sin_addr.s_addr = htonl(INADDR_ANY);

oaiservaddr.sin_port = htons(OAI_PORT);

/* bind address and port to socket */

if(bind(oaisockfd, (strucDBSt sockaddr *)&oaiservaddr, sizeof(oaiservaddr)) == -1)

{

  printf("socket bind error");

  exit(1);

}

if(temp = pthread_create(&oaiudptid,NULL,servthread,NULL)!=0)

{

  printf("can't create thread!");

  exit(1);

}

return 0;

}

void *servthread(void *arg)

{

  char mesg[MAXLINE];


  socklen_t len;

  struct sockaddr_in pcliaddr;

  int n;
  
//  printf("create server thread...");

  while(1){

  	len = sizeof(pcliaddr);

	n = recvfrom(oaisockfd,mesg,MAXLINE,0,(struct sockaddr*)&pcliaddr,&len);

	if(n == -1){

		printf("read message error!");

		exit(1);

	}
	mesg[n] = 0;

	//fputs(mesg,stdout);
   // sendto(oaisockfd,mesg,n,0,(const struct sockaddr*)&pcliaddr,len);
    if(strcmp(mesg,"DBS connection request")==0){
	    char sendmsg[] = "0connection request accepted";
		int tt;
		for(tt = 0;tt<DBS_NUM;tt++){
			if(dbsstat[tt].dbsopen == 0){
				 dbsstat[tt].dbsopen = 1;
				 memcpy((void *)&dbsstat[tt].dbssockaddr,(void*)&pcliaddr,len);
				 sendmsg[0]+=tt;
				 sendto(oaisockfd,sendmsg,sizeof(sendmsg),0,(const struct sockaddr*)&pcliaddr,len);
				 break;
			}
		}
		bsstart = 1;
    }

  }
}



int  init_udpcli(char *addrs)

{
int temp;

bsstart = 0;

oaisockfd = socket(AF_INET, SOCK_DGRAM, 0);
/* init servaddr */

bzero(&oaiservaddr, sizeof(oaiservaddr));

oaiservaddr.sin_family = AF_INET;

oaiservaddr.sin_port = htons(OAI_PORT);

if(inet_pton(AF_INET, addrs, &oaiservaddr.sin_addr) <= 0)
{

printf("[%s] is not a valid IPaddress\n", addrs);

exit(1);

}





//do_cli(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

/* connect to server */

if(connect(oaisockfd, (struct sockaddr *)&oaiservaddr, sizeof(oaiservaddr)) == -1)

{

printf("udp connect error,exit!");

exit(1);

}

if(temp = pthread_create(&oaiudptid,NULL,clithread,NULL)!=0)

{

  printf("can't create thread!");

  exit(1);

}

return 0;

}



void *clithread(void* arg)
{

 // char sendline[MAXLINE],recvline[MAXLINE+1];
  char recvline[MAXLINE];
  int n;

//  printf("create client thread...");

  while(1){
        
        n = recvfrom(oaisockfd,recvline,MAXLINE,0,NULL,NULL);

	if(n == -1){

		printf("read error!");

		exit(1);

	}

	recvline[n] = 0;

	if(n>0&&strcmp(&recvline[1],"connection request accepted")){
		BSid = recvline[0] - '0';
		bsstart = 1;
	}
	
	//fputs(recvline,stdout);
    printf("%s\n",recvline);


  }

}
