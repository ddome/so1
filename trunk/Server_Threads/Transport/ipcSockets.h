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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "../Lib/defines.h"
#include "Transport.h"
#include "../Lib/Error.h"

/* Defines de los Fifo's en particular
*/

#define __DEFAULT_FIFO_MODE__ 0777

#define PACKET_SIZE 1000

/* Funciones
*/

int InitMainIPC(void);

int InitIPC(key_t key);

int WriteIPC(key_t key, void * data, size_t size);

byte * ReadIPC(key_t key);

void CloseIPC(void);

#endif

