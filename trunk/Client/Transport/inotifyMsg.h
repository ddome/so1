#ifndef __IPC_FIFO_H__
#define __IPC_FIFO_H__

/* System Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Project Includes
*/

#include "Transport.h"
#include "../Lib/defines.h"

/* Defines de los Fifo's a utilizar
*/

#define __DEFAULT_FIFO_MODE__ 0777

#define INOT "IN"

/*
*  Posibles mensajes al INotify
*/

#define __NO_DATA__ 1
#define __ENABLE__  2
#define __DISABLE__ 3
#define __EXIT__    4

/* Funciones
*/

int InitINotifyMsg(pid_t pid);

int WriteINotifyMsg(char msg);

char ReadINotifyMsg(void);

void CloseINotifyMsg(void);

#endif

