#ifndef __IPC_INTERFACE_H__
#define __IPC_INTERFACE_H__

#include "ipcFifo.h"
#include <sys/ipc.h>
#include <sys/types.h>

/* Defines generales para todos los IPC's
*/

#define __DEFAULT_PID__         0
#define __DEFAULT_CHILD_PID__   1

typedef struct {
	size_t size;
	int nPacket;
}headerIPC_t;


/* Funciones
*/

int InitMainIPC(void);

int InitIPC(key_t key);

int WriteIPC(void * data, size_t size);

byte * ReadIPC(void);

void CloseIPC(void);


#endif

