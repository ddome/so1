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


/*
*  Defines
*/
#define MAXSENDER 255
#define MAXMSG 255
#define MAXLIST 255

#define __SHUT_DOWN__	 -2

/* Server -> Client respuestas */

#define SR_NEW_USR_ERR 6
#define SR_NEW_USR_OK  7

/* Server -> Client */

#define SR_FIL_MOD     8
#define SR_FIL_ADD     9
#define SR_FIL_REM     10
#define SR_EXT         11

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

int SendFileAddPack( fileT file, byte *data );
int SendFileModPack( fileT file, byte *data );
int SendFileRemPack( fileT file );
int SendDirReq( string dirPath, fileT **fileList, byte ***dataBuffer );
string * SendDirListReq( void );
int SendExitSignal( void );



byte * MakeSessionData( session_t data );
session_t GetSessionData( byte *data );

#endif