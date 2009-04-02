#ifndef _SESSION_H
#define _SESSION_H

/*
 *  System Includes
 */
#include <stdio.h>
#include <string.h>

/*
*  Project Includes
*/
#include "../Lib/genlib.h"
#include "../Lib/defines.h"
#include "../App/fileHandler.h"
#include "../App/Application.h"
#include "../Transport/Transport.h"
#include "../Transport/ipcInterface.h"

/*
*  Defines
*/
#define MAXSENDER 255
#define MAXMSG 255
#define MAXLIST 255

#define __SHUT_DOWN__	 -2

/* Aca poner defines
*  de las acciones que puede realizar el cliente (serian las mismas 
*  q en el server?)
*/ 

/* Paquete de session */

typedef struct {
	char senderID[MAXSENDER];
	char msg[MAXMSG];
	uInt opCode;
	pid_t pid;
	size_t dataSize;	
	byte *data;
} session_t;

/*  Functions */

int InitCommunication(pid_t pid);

pid_t GetRequest(void * data);

int ProcessRequest(byte ** data, pid_t requestPid);


#endif