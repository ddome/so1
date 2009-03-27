   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define ERROR -1

char * mainFifo_Path = "mainFifo";
char * readFifo_PathArray = "0_rd" ;
char * writeFifo_PathArray = "0_wr";

int mainFifo_FD,
    writeFifo_FD,
    readFifo_FD;

typedef struct {
	int dni;
	int esgay;
}cacaT;

typedef struct {
	size_t size;
	int nPacket;
}headerIPC_t;

int
main(void)
{
int i;
	printf("enviando mensaje...");
    char data = 'x';
    headerIPC_t header;
 	cacaT caca;
	caca.dni=5453;
	caca.esgay=1;
    header.size=sizeof(cacaT);
    header.nPacket=0;
    	
  for( i = 0; i < 16; i++)
  {
    header.nPacket = i;
    if(  (readFifo_FD = open("0_rd", O_WRONLY )) < 0)
	    printf("fallo la apertura del fifo");
    
	if(write(readFifo_FD ,&header,sizeof(headerIPC_t)) == -1)
	    printf("no se puede escribir fifo");

	if(write(readFifo_FD ,&caca,sizeof(cacaT)) == -1)
	    printf("no se puede escribir fifo");
getchar();
 }
	    return 0;
   
}

