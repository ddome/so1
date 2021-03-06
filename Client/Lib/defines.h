/*
 *  defines.h
 *  Guarden todas los defines, enums y typedefs generales
 *
 *  Created by Damian Dome on 3/15/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DEFINES_
#define _DEFINES_

#define MAX_DIR_NAME 100
#define MAX_USR_NAME 100
#define MAX_LINE	 120
#define MAX_MSG		 255
#define MAX_LOG_LEN	100

#define __POOL_WAIT__ 50000

enum  {ERROR = -1, OK = 1};

typedef enum boolean{ FALSE = 0, TRUE = 1} boolean;

typedef char * string;

typedef unsigned int uInt;

typedef char byte;

typedef struct 
{
    int opCode;
    char dir[MAX_MSG];
    pid_t pid;
    int status;
}process_t;

#endif

