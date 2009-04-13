/*
*  Project Includes
*/

#include "Server.h"


/*
*  Functions
*/

int
StartListening(void)
{
    int status;
    byte * data;
    process_t process, consoleProcess;
    size_t size = 0;
    status = InitCommunication(__DEFAULT_PID__);
    consoleProcess.opCode = __SPAWN_PROMPT__;

    if(status == ERROR)
    {
	    return ERROR;
    }

    status = SpawnSubProcess(consoleProcess, size,data);
    while(status != ERROR && status != __SHUT_DOWN__)
    {
        data = ReadRequest();

        if(data != NULL)
        {
            /* se manda a que sea procesado en la capa de sesion 
            */
            process = ProcessRequest(&data, &size);
            status = AnalyzeOperation(process, data, size);
        }
        else
        {
            status = ERROR;
        }
    }
    return status;
}

byte *
ReadRequest(void)
{
    byte * data;
    int requestExists = FALSE;
    int status;
    while(!requestExists)
    {
        if( (data = GetRequest()) != NULL)
        {
            requestExists = TRUE;
        }
    }
	
    return data;
}

int
AnalyzeOperation(process_t process, byte * data, size_t size)
{
    int status; 
    if(process.status == ERROR)
    {
        status = ERROR;
    }
    else
    {
        switch(process.opCode)
        {
            case __NOT_SPAWN__:
                status = ProcessSendPack(&data, size);
                break;
            case __SPAWN_DIR__:
                status = SpawnSubProcess(process, size, data);
                break;
            case __SPAWN_DEMAND__:
              status = SpawnSubProcess(process, size, data);
                break;
            case __NO_RESPONSE__:
               // free(data);
                //data = NULL;
                status = OK;
                break;
            default:
                status = ERROR;
                break;
        }
    }
    return status;
}

int
SpawnSubProcess(process_t process, size_t size, byte * data)
{
    pid_t childPid;
    int returnValue = OK;
	
    switch(childPid = fork())
    {
        case ERROR: 
            returnValue = __ERROR_SERVER__;
            break;
        case __ISCHILD__:
            returnValue = StartSubProcess(process);
            break;
        default:
            if(size > 0)
                returnValue = ProcessSendPack(&data, size);
            break;
    }

    return returnValue;
}


int StartSubProcess(process_t process)
{
    int returnValue = OK;
    switch(process.opCode)
    {
	    case __SPAWN_PROMPT__:
	        Prompt();
	        break;
	    case __SPAWN_DIR__:
	        returnValue = StartDirSubServer(process);
	        break;
	    case __SPAWN_DEMAND__:
            returnValue = StartDemandSubServer(process);
	        break;
	    /* Si no era un codigo de operacion valido, se devuelve error
	    */
	    default:
	        returnValue = __ERROR_SERVER__;
	        break;
    }
    
    return returnValue;
}

int StartDirSubServer(process_t reqProcess)
{
    process_t process;
	int status;
    size_t size;
	key_t keyDefault, keyClient;
	byte * data;
    char * aux;
    
    keyClient = ftok(aux = Concat(BK_PATH, reqProcess.dir), reqProcess.pid);
    free(aux);
    status = InitCommunication(keyClient);
    if(status > ERROR)
    {
        keyDefault = ftok(aux = Concat(BK_PATH, reqProcess.dir), __DEFAULT_PID__);
        free(aux);
        status = InitCommunication(keyDefault);
    }
    
    while(status > ERROR && status != __SHUT_DOWN__)
    {
	    data = ReadRequest();
    
	    if(data != NULL)
	    {
	        /* se manda a que sea procesado en la capa de sesion 
	        */
	        process = ProcessRequest(&data, &size);
            /*
            *  Se switchea al key del cliente para escribir
            */
            keyClient = ftok(aux = Concat(BK_PATH, reqProcess.dir), process.pid);
            free(aux);
            if((status = InitCommunication(keyClient)) > ERROR)
	            status = AnalyzeOperation(process, data, size);
            /* Se retorna al IPC default
            */
            if(status > ERROR)
                status = InitCommunication(keyDefault);
	    }
	    else
	    {
	        status = ERROR;
	    }
    }
	
	return status;
}

byte * ReadDirSubServerRequests(void)
{
    byte * data;
    int requestExists = FALSE;
    int status;
    while(!requestExists)
    {
        if( (data = GetRequest()) != NULL)
        {
            requestExists = TRUE;
        }
    }
	
    return data;
}

int StartDemandSubServer(process_t process)
{
    int status;
    char * aux;
    char ble[MAXMSG];
    sprintf(ble, "%d", process.status);
    fopen(ble, "w+");
    key_t key = ftok(aux = Concat(BK_PATH, process.dir), process.status);
    free(aux);
    
    status = InitCommunication(key);
    if(status > ERROR)
    {
        status = SendDirPack(process);
    }
    
    return status;
}































