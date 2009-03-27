/*
*  System Includes
*/

#ifndef _SESSION_H
#define _SESSION_H

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

/* Prompt -> Server operations */
#define PR_DIR_REG 0
#define PR_USR_LST 1
#define PR_ACT_LST 2
#define PR_DIR_USR 3
#define PR_EXT     5

/* Server -> Client operations */
#define SR_FIL_ADD 6
#define SR_FIL_REM 7
#define SR_FIL_MOD 8
#define SR_EXT     9

/* Client -> Server operations */
#define CL_DIR_REQ 10
#define CL_DIR_REM 11
#define CL_FIL_ADD 12
#define CL_FIL_REM 13
#define CL_FIL_MOD 14
#define CL_DIR_LST 15
#define CL_EXT     16

/* Struct */

typedef struct {
	char senderID[MAXSENDER];
	char msg[MAXMSG];
	uInt opCode;
	pid_t pid;
	char *List[MAXLIST];
	byte *data;
} sesion_t;

/*  Functions */

int InitCommunication(pid_t pid);

pid_t GetRequest(void * data);

void ProcessRequest(void * data, pid_t requestPid);

#endif