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
#include "../Lib/Error.h"
#include "../Transport/OutputPipe.h"
#include "DBexec.h"
/*
*  Defines
*/

#define __DEFAULT_PID__	  0

#define BK_PATH "/home/bombax/workspace/so1/Server/backup/"



/* Tipos de procesos disponibles para crearse
*/


#define __NOT_SPAWN__         0
#define __SPAWN_PROMPT__      1
#define __SPAWN_DIR__         2
#define __SPAWN_DEMAND__      3
#define __NO_RESPONSE__       4
#define __SPAWN_OUTPUT__      5
#define __SPAWN_REC_DEMAND__  6
#define __DIR_BROADCAST__     7
#define __SERVER_BROADCAST__  8
#define __KILL_DIR__          9
#define __DIR_BROADCAST_DEMAND__ 10
#define __SPAWN_DEL_SEND__ 11
/* Definiciones para creacion de procesos con fork()
*/

#define __ISCHILD__	  0
#define __SHUT_DOWN__	 -2
#define CHILD_RETURN      2

/* 
*  Interface Functions
*/

int StartListening(void);

byte * ReadRequest(pid_t pid);

int StartSubProcess(process_t* process);

int AnalyzeOperation(process_t process, byte * data, size_t size);

int StartDirSubServer(process_t process);

int StartDemandSubServer(process_t * process);

int StartDemandRecieveSubServer(process_t *  process);

int SpawnSubProcess(process_t process, size_t size, byte * data);

int DirBroadcastMsg(process_t process, size_t size, byte * data);

int StartSendDelSignal(process_t * process);

int KillDirProcess(process_t p);

int InitServerPath();

#endif

