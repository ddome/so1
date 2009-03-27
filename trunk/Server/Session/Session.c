/*
*  Project Includes
*/

#include "Session.h"

/*
*  Functions
*/

typedef struct{
    int dni;
    int esgay;
} cacaT;

int
InitCommunication(pid_t pid)
{
    return InitIPC(pid);
}

pid_t GetRequest(void * data)
{
    return ReadIPC(data);
}

void 
ProcessRequest(void * data, pid_t requestPid)
{
	printf("llego un mensaje!: %d - %s es gay \n", ((cacaT*)data)->dni,((cacaT*)data)->esgay?"SI":"NO");

}



