/*
*  Project Includes
*/

#include "Server.h"

/*
*  Defines
*/

/*
*  Functions
*/

void
StartListening(void)
{
    int status;
    pid_t pid;
    void * data;
    status = InitCommunication(__DEFAULT_PID__);
    while(TRUE)
    {
        pid = ReadRequest(&data);
        /* se manda a que sea procesado en la capa de sesion 
	*/
        ProcessRequest(&data, pid);
    };
    return;
}

pid_t
ReadRequest(void* data)
{
    int requestExists = FALSE;
    pid_t pid;
    while(!requestExists)
    {
        if( (pid = GetRequest(data)) != -1)
        {
            requestExists = TRUE;
        }
    }
	
    return pid;
}

int
SpawnSubProcess(int opCode)
{
    
}
