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
#include "../App/Server.h"
#include "../Transport/Transport.h"
#include "../Transport/ipcInterface.h"

/*
*  Defines
*/
#define MAXSENDER 255
#define MAXMSG 255
#define MAXLIST 255

#define __SHUT_DOWN__	 -2

/* Prompt -> Server operations 
*/

#define PR_EXT     5

/* Server -> Client operations 
*/
#define SR_NEW_USR_ERR 6
#define SR_NEW_USR_OK  7
#define SR_FIL_MOD 8
#define SR_EXT     9

/* Client -> Server operations 
*/
#define CL_NEW_USR 17
#define CL_DIR_REQ 10
#define CL_DIR_REM 11
#define CL_FIL_ADD 12
#define CL_FIL_REM 13
#define CL_FIL_MOD 14
#define CL_DIR_LST 15
#define CL_EXT     16

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

byte * MakeSessionData( session_t data );
session_t GetSessionData( byte *data );

#endif


