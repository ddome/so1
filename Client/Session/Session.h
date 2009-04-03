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

/* Prompt -> Client operations 
 */

#define PR_EXT     1

/* Server -> Client operations 
 */
#define SR_NEW_USR_ERR 2
#define SR_NEW_USR_OK  3
#define SR_FIL_MOD     4
#define SR_FIL_ADD     5
#define SR_FIL_REM     6
#define SR_EXT         7

/* Client -> Server operations 
 */
#define CL_NEW_USR 8
#define CL_DIR_REQ 9
#define CL_DIR_REM 10
#define CL_FIL_ADD 11
#define CL_FIL_REM 12
#define CL_FIL_MOD 13
#define CL_DIR_LST 14
#define CL_EXT     15
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

int SendFileAddPack(  string userName, fileT file, byte *data );
int SendFileModPack(  string userName, fileT file, byte *data );
int SendFileRemPack( string userName, fileT file );
int SendDirReq( string userName, string dirPath, fileT **fileList, byte ***dataBuffer );
int SendDirListReq( string userName );
int SendExitSignal( string userName );

#endif