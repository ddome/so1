
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


int
main(void)
{
	printf("enviando mensaje...");
    char data[8] = "lalalala";
 
    	

    if ( mkfifo(readFifo_PathArray, 0777) == ERROR )
    {
        if(errno != EEXIST)
            return ERROR;
    }
  
  
  if(  readFifo_FD = open(readFifo_PathArray, O_WRONLY  | O_NONBLOCK) < 0)
  	printf("fallo la apertura del fifo");
  	
  
    
    if(write(readFifo_FD,data,strlen(data) +1) == -1)
    	printf("no se puede escribir fifo");
}

