/* Archivo: Server.c
*  -----------------
*  Implementacion de la interface Server.h, la cual maneja la creacion,
*  continuidad y comportamiento de procesos.
*/

/*
*  Project Includes
*/

#include "Server.h"



char *bk_path;

/*
*  Functions
*/

int
StartListening(void)
{
  int status;
  byte * data;
  key_t key;
  size_t size = 0;
  process_t consoleProcess, process, outputProcess;
  key = ftok("/", getpid());

  consoleProcess.opCode = __SPAWN_PROMPT__;
  outputProcess.opCode = __SPAWN_OUTPUT__;

  status = SpawnSubProcess(outputProcess, size, data);
  if(status == CHILD_RETURN)
  {
    return OK;
  }

  do
  {
        status = InitCommunication(__DEFAULT_PID__);
  } while(status == ERROR);

  status = SpawnSubProcess(consoleProcess, size, data);

  if(status == CHILD_RETURN)
  {
    return OK;
  }

  while(status != __SHUT_DOWN__)
  {

    do{
        status = InitCommunication(key);
    }while(status == ERROR);

    data = ReadRequest();
    if(data != NULL)
    {
        status = InitCommunication(__DEFAULT_PID__);
        /* Aca si status == ERROR quiere decir
         q el servidor salio */
        if(status != ERROR)
        {
            /* se manda a que sea procesado en la capa de sesion 
            */
            process = ProcessRequest(&data, &size);
            status = AnalyzeOperation(process, data, size);
        }

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
                returnValue = CHILD_RETURN;
	        break;			
		case __SPAWN_OUTPUT__:
	        PromptReader();
                returnValue = CHILD_RETURN;
	        break;	
	    case __SPAWN_DIR__:
	        returnValue = StartDirSubServer(process);
	        break;
	    case __SPAWN_DEMAND__:
                returnValue = StartDemandSubServer(process);
	        break;
	    case __SPAWN_INOTIFY__:
		    returnValue = StartInotifySubServer(process);
		    returnValue = CHILD_RETURN;
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
    process_t process,inotifyProcess;
    int status = ERROR;
    size_t size;
    key_t keyDefault, keyClient;
    byte * data;
    char * aux;
    keyDefault = ftok(aux = Concat(bk_path,reqProcess.dir), __DEFAULT_PID__);
    free(aux);
    keyClient = ftok(aux = Concat(bk_path,reqProcess.dir), reqProcess.pid);
    free(aux);
    while(status<=ERROR)
    {
        status = InitCommunication(keyDefault);
    }
    
    
    status = InitINotifyMsg(getpid());
    if(status == ERROR)
    {
	    return ERROR;
    }
    
    inotifyProcess.pid = getppid();
    strcpy(inotifyProcess.dir,reqProcess.dir);
    inotifyProcess.opCode=__SPAWN_INOTIFY__;
    status=SpawnSubProcess(inotifyProcess,0,NULL);
    
    if(status==CHILD_RETURN)
	    return OK;
    
    status = SendDirConectionSignal(reqProcess.pid, reqProcess.dir);
    if(status != ERROR)
    {
	do
	{
	    status = InitCommunication(keyClient);
	}while(status <= ERROR);


	while(status != __SHUT_DOWN__)
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
		    do
		    {
			status = InitCommunication(keyClient);
		    } while (status <= ERROR);
		}
		else
		{
		    status = ERROR;
		}
	}
    }
	return status;
}

byte * ReadDirSubServerRequests(void)
{
    byte * data;
    int requestExists = FALSE;

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
    int status = ERROR;
    int requestExists = FALSE;
    byte * data;
    process_t p;
    size_t size;
    char * aux;

    key_t key = ftok(aux = Concat(bk_path, process.dir), getppid());
    free(aux);
    while(status<=ERROR)
    {
	status = InitCommunication(key);
    }
    if(status > ERROR)
    {
		while(!requestExists)
		{
			if( (data = GetRequest()) != NULL)
			{
				p = ProcessRequest(&data, &size);
                if(p.status != ERROR)
                  while(WriteINotifyMsg(__INOTIFY_ENABLE__) == ERROR)
                    ;
				requestExists=TRUE;
			}
		}
    }
    
    return status;
}



static char *
ReadBkPath()
{
	FILE *fd;
	char *path;
	int c;
	
	if( (fd = fopen("config", "r")) == NULL )
		return NULL;
	path = malloc(150);
	
	int i=0;
	while( (c=fgetc(fd)) != '#' ) {
		path[i] = c;
		i++;
	}
	path[i] = '\0';
	
	return path;
	
}

int
InitServerPath()
{
	if( (bk_path = ReadBkPath()) == NULL ) {
		WritePrompt("El archivo config esta dañado");
		return ERROR;
	}	
		
	return OK;
}	


/*int StartDemandSubServer(process_t process)
{
  int status=OK;
  char * aux;

  key_t key = ftok(aux = Concat(BK_PATH, process.dir), getppid());

  free(aux);
  do{
    status = InitCommunication(key);
  }while(status <= ERROR);
  
  if(status > ERROR)
  { 
    status = SendDirPack(process);
  }
    
    
  return status;
}*/

int StartPingServer(pid_t pid, char msg[MAX_MSG])
{
    return 1;
}

int
StartInotifySubServer(process_t process)
{
    char * aux;
    int status;
    aux = Concat(BK_PATH,process.dir);
    
    do
    {
	    status = inotifyWatcher(process);
    }while(status == ERROR);
    
    free(aux);
    return OK;
}
