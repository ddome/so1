#ifndef __IPC_INTERFACE_H__
#define __IPC_INTERFACE_H__

#include "ipcFifo.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <pthread.h>
/* Defines generales para todos los IPC's
*/

#define __DEFAULT_PID__         0
#define __DEFAULT_CHILD_PID__   1

typedef struct {
	size_t size;
	int nPacket;
	int totalPackets;
}headerIPC_t;


/* Funciones
*/

int InitMainIPC(void);

int InitIPC(key_t key);

int WriteIPC(key_t key, void * data, size_t size);

byte * ReadIPC(key_t key);

void CloseIPC(void);

#endif

