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

int
StartListening(void)
{
    int status;
    pid_t pid;
    void * data;
    status = InitCommunication(__DEFAULT_PID__);
    if(status == ERROR)
    {
	    return ERROR;
    }
    status = status && SpawnSubProcess(__SPAWN_PROMPT__, __DEFAULT_PID__, NULL);
    while(status != ERROR && status != __SHUT_DOWN__)
    {
        pid = ReadRequest(&data);
        /* se manda a que sea procesado en la capa de sesion 
        */
        status = ProcessRequest(data, pid);
    }
    return status;
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
SpawnSubProcess(int opCode, pid_t pid, char msg[MAX_MSG])
{
    pid_t childPid;
    int returnValue = OK;
	
    switch(childPid = fork())
    {
        case ERROR: 
            returnValue = ERROR;
            break;
        case __ISCHILD__:
            returnValue = StartSubProcess(opCode, pid, msg);
            break;
        /*  Si es el padre no se hace nada
        */
        default:
            break;
    }

    return returnValue;
}


int StartSubProcess(int opCode, pid_t pid, char msg[MAX_MSG])
{
    int returnValue = OK;
    switch(opCode)
    {
	case __SPAWN_PROMPT__:
	    Prompt();
	    break;
	case __SPAWN_DIR__:
	    returnValue = StartDirSubServer(pid, msg);
	    break;
	case __SPAWN_DEMAND__:
        returnValue = StartDemandSubServer(pid, msg);
	    break;
	/* Si no era un codigo de operacion valido, se devuelve error
	*/
	default:
	    returnValue = ERROR;
	    break;
    }
    
    return returnValue;
}

int StartDirSubServer(pid_t pid, char msg[MAX_MSG])
{
	key_t key;
	key = ftok(msg, pid);
	if(InitCommunication(key) == ERROR)
	{
		return ERROR;
	}
	
	ReadDirSubServerRequests(key);
	
	return OK;
}

int StartDemandSubServer(pid_t pid, char msg[MAX_MSG])
{
    return 1;
}

void ReadDirSubServerRequests(key_t key)
{
    void * data;
    pid_t pid;
    int exit=FALSE;
    while(!exit)
    {
        pid=ReadRequest(data);
        ProcessRequest(data, pid);

    }
}





























