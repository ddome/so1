/* Archivo: Server.c
*  -----------------
*  Implementacion de la interface Server.h, la cual maneja la creacion,
*  continuidad y comportamiento de procesos.
*/

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
  size_t size = 0;
  process_t consoleProcess, process, outputProcess;

  consoleProcess.opCode = __SPAWN_PROMPT__;
  outputProcess.opCode = __SPAWN_OUTPUT__;
  status = InitCommunication(__DEFAULT_PID__);
  if(status == ERROR)
  {
    return ERROR;
  }
  
  status = SpawnSubProcess(consoleProcess, size, data);
  status = SpawnSubProcess(outputProcess, size, data); 
  if(status == CHILD_RETURN)
  {
    return OK;
  }

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
            returnValue = ERROR;
            break;
        case __ISCHILD__:
            returnValue = StartSubProcess(process);
            break;
		default:
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
		case __SPAWN_OUTPUT__:
	        PromptReader();
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
	        returnValue = ERROR;
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
	keyDefault = ftok(aux = Concat(BK_PATH,reqProcess.dir), __DEFAULT_PID__);
	free(aux);
	keyClient = ftok(aux = Concat(BK_PATH,reqProcess.dir), reqProcess.pid);
	free(aux);
	status = InitCommunication(keyDefault);
	
	if(status > ERROR)
	{
		status = SendDirConectionSignal(reqProcess.pid, reqProcess.dir);
	}
	if(status > ERROR)
	{
	    status = InitCommunication(keyClient);
	}
    while(status > ERROR && status != __SHUT_DOWN__)
    {
	    data = ReadRequest();
    
	    if(data != NULL)
	    {
	        /* se manda a que sea procesado en la capa de sesion 
	        */
	        process = ProcessRequest(&data, &size);
			status = InitCommunication(keyDefault);
			if(status > ERROR)
				status = AnalyzeOperation(process, data, size);
			if(status > ERROR )
				status = InitCommunication(keyClient);
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
	int requestExists = FALSE;
	byte * data;
	process_t p;
	size_t size;
    char * aux;
    
    fopen("llegooop 2", "w+");
    key_t key = ftok(aux = Concat(BK_PATH, process.dir), getppid());
    free(aux);
    status = InitCommunication(key);
    if(status > ERROR)
    {
		while(!requestExists)
		{
			if( (data = GetRequest()) != NULL)
			{
				p = ProcessRequest(&data, &size);
				requestExists=TRUE;
				    
			}
		}
    }
    
    return status;
}



int StartPingServer(pid_t pid, char msg[MAX_MSG])
{
    return 1;
}
