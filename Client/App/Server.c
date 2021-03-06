/* Archivo: Server.c
*  -----------------
*  Implementacion de la interface Server.h, la cual maneja la creacion,
*  continuidad y comportamiento de procesos.
*/

/*
*  Project Includes
*/

#include "Server.h"

#include <signal.h>

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
 
  /* Creo el proceso del prompt */
  status = SpawnSubProcess(consoleProcess, size, data);

  if(status == CHILD_RETURN)
  {
    return OK;
  }

  while(status != __SHUT_DOWN__)
  {

    do{
	/* Inicio la comunicacion escuchando pedidos en un canal de lectura propio */
        status = InitCommunication(getpid());
        usleep(__POOL_WAIT__);
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
    usleep(__POOL_WAIT__);
  }
    
  return data;
}

int
AnalyzeOperation(process_t process, byte * data, size_t size)
{

    process_t inotifyProcess;
    
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
            case __SPAWN_SND_DEMAND__:
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
            returnValue = StartSubProcess(process, size, data);
	        exit(EXIT_SUCCESS);
            break;
	default:	        
		break;
    }

    return returnValue;
}


int StartSubProcess(process_t process, size_t size, byte * data)
{
    int returnValue = OK;
    switch(process.opCode)
    {
	    case __SPAWN_PROMPT__:
	        Prompt();
		    kill(getppid(),SIGINT);
            returnValue = CHILD_RETURN;
	        break;			
		case __SPAWN_OUTPUT__:
	        PromptReader();
                returnValue = CHILD_RETURN;
	        break;	
	    case __SPAWN_DEMAND__:
                returnValue = StartDemandSubServer(process);
                printf("Murio el hijo demanda\n");
                returnValue = CHILD_RETURN;
	        break;
	    case __SPAWN_INOTIFY__:
		    returnValue = StartInotifySubServer(process);
		    returnValue = CHILD_RETURN;
	        break;
        case __SPAWN_SND_DEMAND__:
            returnValue = StartDemandSndSubServer(process);
            returnValue = CHILD_RETURN;
	    /* Si no era un codigo de operacion valido, se devuelve error
	    */
	    default:
	        returnValue = ERROR;
	        break;
    }
    
    return returnValue;
}

//DEPRECATED
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
        usleep(__POOL_WAIT__);
    }
    
    
    status = InitINotifyMsg(getpid(),NULL);
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
            usleep(__POOL_WAIT__);
	    }while(status <= ERROR);
    
    
	    while(status != __SHUT_DOWN__ && status != CHILD_RETURN)
	    {
		    data = ReadRequest();
    
		    if(data != NULL)
		    {
		        /* se manda a que sea procesado en la capa de sesion 
		        */
		        process = ProcessRequest(&data, &size);
		        status = InitCommunication(keyDefault);
			    status = AnalyzeOperation(process, data, size);
                if(status != CHILD_RETURN)
                {
                    do
                    {
                        status = InitCommunication(keyClient);
                        usleep(__POOL_WAIT__);
                    } while (status <= ERROR);
                }
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
    process_t inotifyProcess;
        
    /* Me conecto al servidor de transferencia */
    status = ERROR;
    while(status<=ERROR)
    {
	    status = InitCommunication(process.pid);
        usleep(__POOL_WAIT__);
    }
      
    /* Comunicacion con el inotify */
    status = ERROR;
    while(status<=ERROR)
    {
        /* Lo identifico con el pid del proceso principal */
	    status = InitINotifyMsg(getppid(),process.dir);
        usleep(__POOL_WAIT__);
    }
    
     /* Creo el proceso inotify */
    inotifyProcess.pid = getppid();
    strcpy(inotifyProcess.dir,process.dir);
    inotifyProcess.opCode=__SPAWN_INOTIFY__;
    SpawnSubProcess(inotifyProcess,0,NULL);                

    if(status > ERROR)
    {
		while(!requestExists)
		{
			if( (data = GetRequest()) != NULL)
			{
				p = ProcessRequest(&data, &size);
                if(p.status != ERROR) {
    

                     while(WriteINotifyMsg(__INOTIFY_ENABLE__) == ERROR) {
                       usleep(__POOL_WAIT__);
                    }

                }
                       
				requestExists=TRUE;
			}
		}
    }
    
    return status;
}

/* Recibe el archivo broadcast */
int 
StartDemandSndSubServer(process_t process)
{
    int status;
    char * aux;
    pid_t pid;
    boolean requestExists = FALSE;
    byte *data;
    size_t size;
    process_t p;
 
    status = ERROR;
    /* Deshabilito el inotify */
   while(status<=ERROR)
    {
        /* Lo identifico con el pid del proceso principal */
	    status = InitINotifyMsg(getppid(),process.dir);
        usleep(__POOL_WAIT__);
    }
    
    status = ERROR;
    while(WriteINotifyMsg(__INOTIFY_DISABLE__) == ERROR) {
        usleep(__POOL_WAIT__);
    }

    pid = process.pid;
    status = ERROR;
    do
    {
        status = InitCommunication(process.pid);
        usleep(__POOL_WAIT__);
    } while(status <= ERROR);
    
   /* Recibo el pedido */
  if(status > ERROR)
    {
		while(!requestExists)
		{
			if( (data = GetRequest()) != NULL)
			{
				p = ProcessRequest(&data, &size);
                if(p.status != ERROR) {
                    
                    /* Habilito nuevamente el inotify */
                    
                    sleep(3);
                    
                     while(WriteINotifyMsg(__INOTIFY_ENABLE__) == ERROR) {
                       usleep(__POOL_WAIT__);
                    }
                }
                       
				requestExists=TRUE;
			}
		}
    }    
    
   
    
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
		fflush(stdout);
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

int
DirRemoveSync(string user, string dir)
{
   

    int status = ERROR;
    /* Deshabilito el inotify */
   while(status<=ERROR)
    {
        /* Lo identifico con el pid del proceso principal */
	    status = InitINotifyMsg(getppid(),dir);
        usleep(__POOL_WAIT__);
    }    
    status = ERROR;
    while(WriteINotifyMsg(__INOTIFY_EXIT__) == ERROR) {
        usleep(__POOL_WAIT__);
    }
    
    SendDirRemoveSignal(user,dir,getppid());         
}

int StartPingServer(pid_t pid, char msg[MAX_MSG])
{
    return 1;
}

int
StartInotifySubServer(process_t process)
{  
    int status = inotifyWatcher(process);

    return status;
}


void
KillInotify(pid_t pid, char *dir)
{
   int status = ERROR;
    /* Deshabilito el inotify */
   while(status<=ERROR)
    {
        /* Lo identifico con el pid del proceso principal */
	    status = InitINotifyMsg(pid,dir);
        usleep(__POOL_WAIT__);
    }    

    status = ERROR;
    while(WriteINotifyMsg(__INOTIFY_EXIT__) == ERROR) {
        usleep(__POOL_WAIT__);
    }
                
}

