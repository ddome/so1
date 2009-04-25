/*
*  Project Includes
*/

#include "Server.h"


char * bk_path;

/*
*  Functions
*/

int
StartListening(void)
{
    int status;
    byte * data;
    key_t key;
    process_t process, consoleProcess, outputProcess;
    size_t size = 0;
    status = InitCommunication(__DEFAULT_PID__);
    consoleProcess.opCode = __SPAWN_PROMPT__;
	outputProcess.opCode = __SPAWN_OUTPUT__;

    status = SpawnSubProcess(outputProcess, size,data);
    if(status == CHILD_RETURN)
	return OK;

    if((status = InitCommunication(__DEFAULT_PID__)) <= ERROR)
    {
        return ERROR;
    }

    status = SpawnSubProcess(consoleProcess, size,data);
    if(status == CHILD_RETURN)
	return OK;

    while(status != __SHUT_DOWN__)
    {
        if((status = InitCommunication(__DEFAULT_PID__)) > ERROR)
        {
            data = ReadRequest();
    
            if(data != NULL)
            {
                /* se manda a que sea procesado en la capa de sesion 
                */
                process = ProcessRequest(&data, &size);
                key = ftok("/", process.pid);

                status = InitCommunication(key);
                if(status > ERROR)                    
                    status = AnalyzeOperation(process, data, size);
            }
            else
            {
                status = ERROR;
            }
            if(status <= ERROR)
                WritePrompt("Se ha producido un error interno");
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
            case __DIR_BROADCAST__:
                status = DirBroadcastMsg(process, size, data);
                break;
	        case __KILL_DIR__:
				status = KillDirProcess(process);
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
    int returnValue = OK,aux;
	
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
            {
             //   if(process.opCode == __SPAWN_DEMAND__)
             //       wait(&aux);
                if( process.opCode!=__NO_RESPONSE__ && process.opCode != __DIR_BROADCAST__)
                {
                    returnValue = ProcessSendPack(&data, size);
                }
            }
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
            returnValue = CHILD_RETURN;
	        break;
	    case __SPAWN_DEMAND__:
                returnValue = StartDemandSubServer(process);
                returnValue = CHILD_RETURN;
	        break;
        case __SPAWN_REC_DEMAND__:
            returnValue = StartDemandRecieveSubServer(process);
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
    
    keyClient = ftok(aux = Concat(bk_path, reqProcess.dir), reqProcess.pid);
    free(aux);
    do{
        status = InitCommunication(keyClient);
    } while (status <= ERROR);
    if(status > ERROR)
    {
        keyDefault = ftok(aux = Concat(bk_path, reqProcess.dir), __DEFAULT_PID__);
        free(aux);
        status = InitCommunication(keyDefault);
    }
    
    while(status != __SHUT_DOWN__ && status != CHILD_RETURN)
    {
        /* Se retorna al IPC default
        */
        status = InitCommunication(keyDefault);
	    data = ReadRequest();
    
	    if(data != NULL)
	    {
	        /* se manda a que sea procesado en la capa de sesion 
	        */
	        process = ProcessRequest(&data, &size);
            /*
            *  Se switchea al key del cliente para escribir
            */
            keyClient = ftok(aux = Concat(bk_path, reqProcess.dir), process.pid);
            free(aux);
            if((status = InitCommunication(keyClient)) > ERROR)
	            status = AnalyzeOperation(process, data, size);
    
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
    int status=OK;
    char * aux;

    key_t key = ftok(aux = Concat(bk_path, process.dir), process.status);

    free(aux);
    do{
        status = InitCommunication(key);
    }while(status <= ERROR);
    if(status > ERROR)
    { 
        status = SendDirPack(process);
    }
    
    
    return status;
}

int StartDemandRecieveSubServer(process_t process)
{
  int status = ERROR;
  int requestExists = FALSE;
  byte * data;
  process_t p;
  size_t size;
  char * aux;

  key_t key = ftok(aux = Concat(bk_path, process.dir), process.status);
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
        requestExists=TRUE;
      }
    }
  }
    
  return status;
}

int
DirBroadcastMsg(process_t process, size_t size, byte * data)
{

    int status = OK;
    key_t key;
    int cantUsersInDir, i;
    int * userPidArray;
    
    /* Se obtienen la cantidad de usuarios conectados al directorio.
    ** Si hay 1 solo, no se manda broadcast, pues el unico que hay
    ** es el que hizo la modificacion del archivo en primera instancia.
    */
    if((cantUsersInDir = CantUsersLinkToDir(process.dir)) < 2)
    {
      return OK;
    }
    
    /* Se almacenan en userPidArray los pids de dichos usuarios
    *
    */
    userPidArray = PIDsLinkToDir(process.dir);

    if(userPidArray == NULL)
      return ERROR;
    
    for(i = 0; i < cantUsersInDir; i++)
    {
        /* Se envia el mensaje a todos, excepto al cliente que
        *  genero el broadcast a traves de una modificacion del directorio.
        */
        if(userPidArray[i] != process.pid)
        {
            /* Se genera el key correspondiente al usuario, y se switchea 
            *  la comunicacion a ese canal.
            */
            key = ftok(process.dir, userPidArray[i]);
            char a[20];
            sprintf(a, "key%d %d", i, key);
            fopen(a,"w+");
            do
            {
                status = InitCommunication(key);
            } while(status <= ERROR);
            
            /* Se envia el mensaje
            */
            fopen("mandandoarchivo","w+");
            status = ProcessSendPack(&data, size);
        }
    }

    /* Antes de finalizar se vuelve al canal default de comunicación
    */
    key = ftok(process.dir, __DEFAULT_PID__);
    status = InitCommunication(key);
    
    return status;
    
}


int
KillDirProcess(process_t p)
{
	//SendClientDirRem(p)
	return OK;
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
	else
		return OK;
}	
	























