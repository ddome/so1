#ifndef __IPC_INTERFACE_H__
#define __IPC_INTERFACE_H__

/* Aca se podria determinar a que funcion llama en funcion de una variable
*  de entorno o argumento en linea de comando, determinando que ipc se usa.
*  Por ahora solo fifos.
*/

#include "ipcFifo.h"

/* Defines generales para todos los IPC's
*/

#define __DEFAULT_PID__         0
#define __DEFAULT_CHILD_PID__   1


/* Funciones
*/

int InitMainIPC(void);

int InitIPC(int pid);

int WriteIPC(void * data, size_t size);

pid_t ReadIPC(void * data);

void CloseIPC(void);


#endif
