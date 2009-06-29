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

    if((status = InitCommunication(__DEFAULT_PID__)) <= ERROR)
    {
        return ERROR;
    }
    /*prompt*/
    status = SpawnSubProcess(consoleProcess, size,data);
    if(status == CHILD_RETURN)
	return OK;

    while(status != __SHUT_DOWN__)
    {
        if((status = InitCommunication(__DEFAULT_PID__)) > ERROR)
        {
	    /* lee pedidos  bloquea */
            data = ReadRequest(__DEFAULT_PID__);
    
            if(data != NULL)
            {
                /* se manda a que sea procesado en la capa de sesion 
                */
                process = ProcessRequest(&data, &size);
		        /* Contesto a esta direccion */
                status = InitCommunication(process.pid);
                if(status > ERROR)                    
                    status = AnalyzeOperation(process, data, size);
            }
            else
            {
                status = ERROR;
            }
            if(status <= ERROR)
                printf("Se ha producido un error interno");
        }
    }
    return status;
}

byte *
ReadRequest(pid_t pid)
{
    byte * data;
    int requestExists = FALSE;
    int status;
    while(!requestExists)
    {
        if( (data = GetRequest(pid)) != NULL)
        {
            requestExists = TRUE;
        }
        usleep(__POOL_WAIT__);
    }
	
    return data;
}

int
AnalyzeOperation(process_t process, byte * data, size_t size)
{
    int status; 
    process_t * p;
    if(process.status == ERROR)
    {
        status = ERROR;
    }
    else
    {
        switch(process.opCode)
        {
            case __NOT_SPAWN__:
                status = ProcessSendPack(process.pid, &data, size);
                break;
            case __SPAWN_DIR__://deprecated
                status = SpawnSubProcess(process, size, data);
                break;
            case __SPAWN_DEMAND__:
              status = SpawnSubProcess(process, size, data);
                break;
            case __SPAWN_DEL_SEND__:
              status = SpawnSubProcess(process, size, data);
                break;
            case __DIR_BROADCAST__:
                p = malloc(sizeof(process_t));
                memcpy(p, &process, sizeof(process_t));
               /* status = */StartSubProcess(p);
                break;
	        case __KILL_DIR__://deprecated
				status = KillDirProcess(process);
				break;
            case __SPAWN_REC_DEMAND__:
                status = SpawnSubProcess(process, size, data);
                break;
            case __SPAWN_DIR_DEL_SEND__:
                status = SpawnSubProcess(process, size, data);  
                break;
            case __SPAWN_DIR_NEW_SEND__:
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
    int returnValue = OK,aux;
    pthread_t newThread;

    process_t * p = malloc(sizeof(process_t));
    memcpy(p, &process, sizeof(process_t));
    pthread_create(&newThread, NULL, (void (*)(void*)) StartSubProcess, p);
    usleep(5000000);
    if(size > 0 )
    {
        if( process.opCode == __NOT_SPAWN__)
        {
            returnValue = ProcessSendPack(process.pid, &data, size);
        }
    }



    return returnValue;
}


int StartSubProcess(process_t * process)
{
    int returnValue = OK;
    switch(process->opCode)
    {
	    case __SPAWN_PROMPT__:
	        Prompt();
		returnValue = CHILD_RETURN;
	        break;
		case __SPAWN_OUTPUT__:
	        PromptReader();
		returnValue = CHILD_RETURN;
	        break;	
	    case __DIR_BROADCAST__:
	        StartTransferSubServer(process);
            returnValue = CHILD_RETURN;
	        break;
	    case __SPAWN_DEMAND__:
                returnValue = StartDemandSubServer(process);
                returnValue = CHILD_RETURN;
	        break;
        case __SPAWN_REC_DEMAND__:
            StartDemandRecieveSubServer(process);
            returnValue = CHILD_RETURN;
                break;
        case __SPAWN_DEL_SEND__:
            StartSendDelSignal(process);
            returnValue = CHILD_RETURN;
                break;
        case __SPAWN_DIR_DEL_SEND__:
            StartDirDelBroadcast(process);
            returnValue = CHILD_RETURN;
                break;        
        case __SPAWN_DIR_NEW_SEND__:
            StartDirNewBroadcast(process);
            returnValue = CHILD_RETURN;
                break;                   
	    /* Si no era un codigo de operacion valido, se devuelve error
	    */
	    default:
	        returnValue = __ERROR_SERVER__;
	        break;
    }
    
    return returnValue;
}

int
StartSendDelSignal(process_t  *process)
{
    int status=OK;
    char * aux;
    pid_t client_pid;
    fileT file;
    char *path;
    char *user;
    int i;
    int *userPidArray;

    int cantUsersInDir = CantUsersLinkToDir(process->dir);

    user = GetPIDToUserName(process->aux_pid);

   /* Se almacenan en userPidArray los pids de dichos usuarios
   *
   */
   userPidArray = PIDsLinkToDir(process->dir);

    for(i=0;i<cantUsersInDir;i++){
        /* Si es distinto del pid del usuario que me lo mando */
        if( userPidArray[i] != process->aux_pid ) {
            user = GetPIDToUserName(userPidArray[i]);        
            do{
                        status = InitCommunication(userPidArray[i]);
                        usleep(__POOL_WAIT__);
                }while(status <= ERROR);

                SendFileRemTransferSignal(process,process->file, userPidArray[i]);
        }
    }

   return status;
}

int
StartDirBroadcast(process_t * process)
{
    int status=OK;

    int i;
    int *userPidArray;
    char *user;

    int cantUsersInDir = CantUsersLinkToDir(process->dir);

    user = GetPIDToUserName(process->aux_pid);

   /* Se almacenan en userPidArray los pids de dichos usuarios
   *
   */
   userPidArray = PIDsLinkToDir(process->dir);

    for(i=0;i<cantUsersInDir;i++){
        /* Si es distinto del pid del usuario que me lo mando */
        if( userPidArray[i] != process->aux_pid ) {
            user = GetPIDToUserName(userPidArray[i]);
            do{
                        status = InitCommunication(userPidArray[i]);
                        usleep(__POOL_WAIT__);
                }while(status <= ERROR);
                
                SendDirDelSignal(process,process->file, userPidArray[i]);
        }            
    }
       
   return status;
}

int
StartDirNewBroadcast(process_t * process)
{
    int status=OK;

    int i;
    int *userPidArray;
    char *user;

    int cantUsersInDir = CantUsersLinkToDir(process->dir);

    user = GetPIDToUserName(process->aux_pid);

    /* Se almacenan en userPidArray los pids de dichos usuarios
    *
    */
    userPidArray = PIDsLinkToDir(process->dir);

    for(i=0;i<cantUsersInDir;i++){
        /* Si es distinto del pid del usuario que me lo mando */
        if( userPidArray[i] != process->aux_pid ) {
            user = GetPIDToUserName(userPidArray[i]);
            do{
                        status = InitCommunication(userPidArray[i]);
                        usleep(__POOL_WAIT__);
                }while(status <= ERROR);

                SendDirAddSignal(process,process->file, userPidArray[i]);
        }
    }

   return status;
}

int
StartDirDelBroadcast(process_t *process)
{
    int status=OK;

    int i;
    int *userPidArray;
    char *user;

    int cantUsersInDir = CantUsersLinkToDir(process->dir);

    user = GetPIDToUserName(process->aux_pid);


    /* Se almacenan en userPidArray los pids de dichos usuarios
    *
    */
    userPidArray = PIDsLinkToDir(process->dir);

    for(i=0;i<cantUsersInDir;i++){
        /* Si es distinto del pid del usuario que me lo mando */
        if( userPidArray[i] != process->aux_pid ) {
            user = GetPIDToUserName(userPidArray[i]);
            fflush(stdout);
            do{
                        status = InitCommunication(userPidArray[i]);
                        usleep(__POOL_WAIT__);
                }while(status <= ERROR);

                SendDirDelSignal(process,process->file, userPidArray[i]);
        }
    }

   return status;
}

int StartDemandSubServer(process_t * process)
{
    int status=OK;
    char * aux;
    pid_t client_pid;

    /* Guardo el pid del cliente a quien avisar */
    client_pid = process->pid;
    /* Voy a transmitir usando el pid del proceso hijo creado */
    process->pid = pthread_self();
        fflush(stdout);
    /* Inicio el canal de comunicacion para la transferencia */
    do{
            status = InitCommunication(pthread_self());
            usleep(__POOL_WAIT__);
    }while(status <= ERROR);

    fflush(stdout);

    sleep(1);

    /* Le aviso al cliente que empiezo a transmitir y por donde */

    fflush(stdout);
    do{
            status = InitCommunication(client_pid);
            usleep(__POOL_WAIT__);
    }while(status <= ERROR);

    fflush(stdout);  

    SendStartTransfer(process, client_pid);
        fflush(stdout);

    sleep(1);
    /* Compienzo a transmitir */

    fflush(stdout);
    do{
            status = InitCommunication(process->pid);
            usleep(__POOL_WAIT__);
    }while(status <= ERROR);

    SendDirPack(process, process->pid);

   return status;
}

/* Manda un archivo broadcast */
int StartTransferSubServer(process_t * process)
{
    int status=OK;
    char * aux;
    pid_t client_pid;
    fileT file;
    char *path;
    char *fileName;

    /* Guardo el pid del cliente a quien avisar */
    client_pid = process->pid;
    /* Voy a transmitir usando el pid del proceso hijo creado */
    process->pid = pthread_self();
        fflush(stdout);
    /* Inicio el canal de comunicacion para la transferencia */
    do{
            status = InitCommunication(pthread_self());
            usleep(__POOL_WAIT__);
    }while(status <= ERROR);


    sleep(1);

    /* Le aviso al cliente que empiezo a transmitir y por donde */

    fflush(stdout);
    do{
            status = InitCommunication(client_pid);
            usleep(__POOL_WAIT__);
    }while(status <= ERROR);

    SendStartFileTransfer(process, client_pid);

    sleep(1);
    /* Compienzo a transmitir */

    fflush(stdout);
    do{
            status = InitCommunication(process->pid);
            usleep(__POOL_WAIT__);
    }while(status <= ERROR);	   

    path = GetPathFromBackup(process->dir);
    fileName = GetFileName(process->dir);

    file = process->file;

    SendFile(process,file, process->pid);

    fflush(stdout);
    return status;
}


int StartDemandRecieveSubServer(process_t* process)
{
  int status = ERROR;
  int requestExists = FALSE;
  byte * data;
  process_t p;
  size_t size;
  char * aux;
  FILE * f;
  InitBD();/*Mirar para threads!!!!*/
  char *user;
  int *userPidArray;
  int i;

   /* Se obtienen la cantidad de usuarios conectados al directorio.
    ** Si hay 1 solo, no se manda broadcast, pues el unico que hay
    ** es el que hizo la modificacion del archivo en primera instancia.
    */
  int cantUsersInDir = CantUsersLinkToDir(process->dir);
 
   /* Se almacenan en userPidArray los pids de dichos usuarios
   *
   */
   userPidArray = PIDsLinkToDir(process->dir);

  while(status<=ERROR)
  {
    status = InitCommunication(process->pid);
    usleep(__POOL_WAIT__);
  }
  
  if(status > ERROR)
  {
  /* Recibo el archivo nuevo */
    while(!requestExists)
    {
      if( (data = GetRequest(process->pid)) != NULL)
      {
        p = ProcessRequest(&data, &size);
        requestExists=TRUE;
        user = GetPIDToUserName(process->aux_pid);
        
        for(i=0;i<cantUsersInDir;i++){
            /* Si es distinto del pid del usuario que me lo mando */
            if( userPidArray[i] != process->aux_pid ) {
                user = GetPIDToUserName(userPidArray[i]);              
                fflush(stdout);
                p.opCode = __DIR_BROADCAST__;
                p.pid = userPidArray[i];
                SpawnSubProcess(p, 0,NULL);
            }            
        }
           
      }
      usleep(__POOL_WAIT__);
    }
  }
  
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
		printf("El archivo config esta dañado");
		return ERROR;
	}
	else
		return OK;
}	
	























