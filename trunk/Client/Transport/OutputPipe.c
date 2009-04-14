/*
 *  PromptPipe.c
 *  test
 *
 *  Created by Damian Dome on 4/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


/* System includes */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include  <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>


/* Project includes */

#include "../Lib/defines.h"
#include "../Lib/genlib.h"
#include "OutputPipe.h"
char * path=NULL;

int
MakePath(pid_t pid)
{
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return ERROR;

    sprintf(path,"%s_prompt_%d_wr",_DEFAULT_PATH_,(int)pid);

    return OK;
}

int
InitPromptCommunication(pid_t pid)
{
	if(MakePath(pid)==ERROR)
	    return ERROR;	
	
	if( open(path, O_RDWR) >= 0 ) {
		remove(path);
	}	
		
	return mkfifo(path,_FIFO_MODE_);	
}

int
WritePrompt(string msg)
{
	int fd;
	int size;
	
	if( ( fd = open(path, O_RDWR) ) < 0 ) {
		return ERROR;
	}
	
	if( msg == NULL ) {
		return ERROR;
	}
	
	size = strlen(msg)+1;
	
	if( write(fd,msg,size) == -1 ) {
		return ERROR;
	}
	
	return OK;
}	
	
string
ReadMessage(void)
{
	int fd;
	char msg[MAX_PROMPT_MSG];
	
	if( ( fd = open(path,O_RDWR) ) < 0 ) {
		return NULL;
	}
	
	read(fd,msg,MAX_PROMPT_MSG);	
		
	/* Me aseguro de que sea un string */
	msg[MAX_PROMPT_MSG-1] = '\0'; 
		
	return CreateString(msg);
}
	
	







