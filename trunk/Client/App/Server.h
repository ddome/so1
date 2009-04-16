
/* Archivo: Server.h
*  -----------------
*  Interface Server.h, la cual maneja la creacion,
*  continuidad y comportamiento de procesos.
*/

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

#define BK_PATH "/home/damian/Desktop/so1/Server/backup/"

/* Tipos de procesos disponibles para crearse
*/
#define __NOT_SPAWN__         0
#define __SPAWN_PROMPT__      1
#define __SPAWN_DIR__         2
#define __SPAWN_DEMAND__      3
#define __SPAWN_INOTIFY__     4
#define __SPAWN_PING__        5
#define __NO_RESPONSE__       6
#define __SPAWN_OUTPUT__      7
#define __SPAWN_SND_DEMAND__  8

/* Definiciones para creacion de procesos con fork()
*/

#define __ISCHILD__	      0
#define __SHUT_DOWN__	 -2
#define CHILD_RETURN      2



int StartListening(void);

byte * ReadRequest(void);

int StartSubProcess(process_t process);

int AnalyzeOperation(process_t process, byte * data, size_t size);

byte * ReadDirSubServerRequests(void);

int StartDirSubServer(process_t process);

int StartDemandSubServer(process_t process);

int SpawnSubProcess(process_t process, size_t size, byte * data);

int StartPingServer(pid_t pid, char msg[MAX_MSG]);

#endif
