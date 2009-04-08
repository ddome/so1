#ifndef __SERVER_H__
#define __SERVER_H__

/*
*  System Includes
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>

/*
*  Project Includes
*/

#include "Prompt.h"
#include "Application.h"
#include "../Lib/defines.h"
#include "../Session/Session.h"

/*
*  Defines
*/

#define __DEFAULT_PID__	  0

/* Tipos de procesos disponibles para crearse
*/


#define __NOT_SPAWN__     0
#define __SPAWN_PROMPT__  1
#define __SPAWN_DIR__     2
#define __SPAWN_DEMAND__  3

/* Definiciones para creacion de procesos con fork()
*/

#define __ISCHILD__	  0
#define __SHUT_DOWN__	 -2


/* 
*  Interface Functions
*/

int StartListening(void);

byte * ReadRequest(void);

int StartSubProcess(int opCode, pid_t pid, char msg[MAX_MSG]);

int AnalyzeOperation(process_t process, byte * data, size_t size);

byte * ReadDirSubServerRequests(void);

int StartDirSubServer(pid_t pid, char msg[MAX_MSG]);

int StartDemandSubServer(pid_t pid, char msg[MAX_MSG]);

int SpawnSubProcess(int opCode, pid_t pid, char msg[MAX_MSG]);

#endif

