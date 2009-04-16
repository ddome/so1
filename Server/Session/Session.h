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
#include "../Lib/Error.h"

/*
*  Defines
*/
#define MAXSENDER 255
#define MAXMSG 255
#define MAXLIST 255

#define __SHUT_DOWN__	 -2

/* Prompt -> Server operations 
*/

#define PR_EXT     1

/* Server -> Client operations 
*/
#define SR_CONECT_OK   16
#define SR_NEW_USR_ERR 2
#define SR_NEW_USR_OK  3
#define SR_FIL_MOD     4
#define SR_FIL_ADD     5
#define SR_FIL_REM     6
#define SR_EXT         7
#define SR_DIR_ADD     18
#define SR_DIR_REQ_OK  19
#define SR_DIR_CON_OK  20
#define SR_DIR_TRANS   22
#define SR_DIR_LST     23
#define SR_DIR_REM 25

//Agregar Sr_DIr-CON-ERROR!!!!!!!!!!1111

/* Client -> Server operations 
*/
#define CL_NEW_CON 17
#define CL_NEW_USR 8
#define CL_DIR_REQ 9
#define CL_DIR_REM 10
#define CL_FIL_ADD 11
#define CL_FIL_REM 12
#define CL_FIL_MOD 13
#define CL_DIR_LST 14
#define CL_EXT     15
#define CL_DIR_CON 21

/* Paquete de session */

typedef struct {
	char senderID[MAXSENDER];
	char msg[MAXMSG];
	uInt opCode;
	pid_t pid;
	size_t dataSize;	
	byte *data;
} session_t;

typedef struct
{
    int opCode;
    char dir[MAX_MSG];
    pid_t pid;
    int status;
} process_t;


/*  Functions */

int InitCommunication(pid_t pid);

byte * GetRequest(void);

process_t ProcessRequest(byte ** data, size_t * size);

int SendDirPack(process_t process);

int ProcessSendPack(byte ** data, size_t size);

int  SendConectionSignal(  pid_t pid );

int SendFileAddPack( string userName, fileT file, byte *data );

#endif


