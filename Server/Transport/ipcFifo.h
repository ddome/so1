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
#include "../Lib/defines.h"
#include "Transport.h"


/* Defines de los Fifo's en particular
*/

#define __DEFAULT_FIFO_MODE__ 0777

/* Funciones
*/

int InitMainIPC(void);

int InitIPC(key_t key);

int WriteIPC(void * data, size_t size);

pid_t ReadIPC(void * data);

void CloseIPC(void);

#endif