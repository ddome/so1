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
            data = ReadRequest();
    
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
                status = StartSubProcess(process);
                break;
	        case __KILL_DIR__://deprecated
				status = KillDirProcess(process);
				break;
          case __SPAWN_REC_DEMAND__:
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
	
    switch(childPid = fork())
    {
        case ERROR: 
            returnValue = __ERROR_SERVER__;
            break;
        case __ISCHILD__:
            returnValue = StartSubProcess(process);
	        exit(EXIT_SUCCESS);
            break;
        default:
            if(size > 0 )
            {
             //   TODO ver si no hace falta aca para recieve demand--resolved si
                
                if( process.opCode!=__SPAWN_REC_DEMAND__  && process.opCode!=__SPAWN_DEMAND__ && process.opCode!=__NO_RESPONSE__ && process.opCode != __DIR_BROADCAST__ && process.opCode != __DIR_BROADCAST_DEMAND__ && process.opCode != __SPAWN_DEL_SEND__)
                {
				/* child pid*/
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
        usleep(__POOL_WAIT__);
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
            if(process.status != __SHUT_DOWN__){
                /*
                *  Se switchea al key del cliente para escribir
                */
                keyClient = ftok(aux = Concat(bk_path, reqProcess.dir), process.pid);
                free(aux);
                if((status = InitCommunication(keyClient)) > ERROR)
	                status = AnalyzeOperation(process, data, size);
            }
            else{
                status = __SHUT_DOWN__;
            }
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
        usleep(__POOL_WAIT__);
    }
	
    return data;
}

int
StartSendDelSignal(process_t process)
{
	    int status=OK;
	    char * aux;
	    pid_t client_pid;
	    fileT file;
	    char *path;
	    char *user;
	    int i;
	    int *userPidArray;
	    
	    int cantUsersInDir = CantUsersLinkToDir(process.dir);
        printf("Tengo %d clientes asociados al directorio %s\n",cantUsersInDir,process.dir);
        fflush(stdout);
		
		user = GetPIDToUserName(process.aux_pid);
        printf("Ahora se lo tengo que mandar a todos salvo a %s\n",user);
        
   /* Se almacenan en userPidArray los pids de dichos usuarios
   *
   */
   userPidArray = PIDsLinkToDir(process.dir);
        
        for(i=0;i<cantUsersInDir;i++){
            /* Si es distinto del pid del usuario que me lo mando */
            if( userPidArray[i] != process.aux_pid ) {
                user = GetPIDToUserName(userPidArray[i]);
                printf("Se lo mando a %s\n",user);                
                fflush(stdout);           
                do{
		            status = InitCommunication(userPidArray[i]);
		            usleep(__POOL_WAIT__);
	            }while(status <= ERROR);
	            
	            SendFileRemTransferSignal(process,process.file);
            }            
       }
       
   return status;
}
int StartDemandSubServer(process_t process)
{
	    int status=OK;
	    char * aux;
	    pid_t client_pid;

	    //key_t key = ftok(aux = Concat(bk_path, process.dir), process.status);//DEPRECATED
	    //free(aux);
        /* Guardo el pid del cliente a quien avisar */
        client_pid = process.pid;
        /* Voy a transmitir usando el pid del proceso hijo creado */
	    process.pid = getpid();
		fflush(stdout);
	    /* Inicio el canal de comunicacion para la transferencia */
	    do{
		    status = InitCommunication(getpid());
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
	       
	    SendStartTransfer(process);
		fflush(stdout);

        sleep(1);
	   /* Compienzo a transmitir */
	   
	   fflush(stdout);
	    do{
		    status = InitCommunication(process.pid);
		    usleep(__POOL_WAIT__);
	    }while(status <= ERROR);	   
	   
	   SendDirPack(process);		
	    
   return status;
}

/* Manda un archivo broadcast */
int StartTransferSubServer(process_t process)
{
	    int status=OK;
	    char * aux;
	    pid_t client_pid;
	    fileT file;
	    char *path;
	    char *fileName;

	    //key_t key = ftok(aux = Concat(bk_path, process.dir), process.status);//DEPRECATED
	    //free(aux);
        /* Guardo el pid del cliente a quien avisar */
        client_pid = process.pid;
        /* Voy a transmitir usando el pid del proceso hijo creado */
	    process.pid = getpid();
		fflush(stdout);
	    /* Inicio el canal de comunicacion para la transferencia */
	    do{
		    status = InitCommunication(getpid());
		    usleep(__POOL_WAIT__);
	    }while(status <= ERROR);
	    
	    printf("Iniciada la comunicacion para la transferencia en %d\n",getpid());
		fflush(stdout);
		
		sleep(1);
		
	    /* Le aviso al cliente que empiezo a transmitir y por donde */
	    
	    fflush(stdout);
	    do{
		    status = InitCommunication(client_pid);
		    usleep(__POOL_WAIT__);
	    }while(status <= ERROR);
	    
	    printf("Iniciada la comunicacion con el cliente %d\n",client_pid);	    
	    fflush(stdout);  
	       
	    SendStartFileTransfer(process);
	    
	    printf("Le avise al cliente que puede empezar a escuchar en %d\n",process.pid);
		fflush(stdout);


        sleep(1);
	   /* Compienzo a transmitir */
	   
	   fflush(stdout);
	    do{
		    status = InitCommunication(process.pid);
		    usleep(__POOL_WAIT__);
	    }while(status <= ERROR);	   
	   
	    path = GetPathFromBackup(process.dir);
        fileName = GetFileName(process.dir);
        
        file = NewFileT(path,fileName);
        
	   	printf("Mandando el archivo %s %s....",path,fileName);
		fflush(stdout);
	   
	    SendFile(process,file);
	   
	   	printf("OK\n");
		fflush(stdout);		
	    
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
  FILE * f;
  InitBD();/*Mirar para threads!!!!*/
  char *user;
  int *userPidArray;
  int i;

  printf("Voy a crear la comunicacion\n");
  fflush(stdout);

   /* Se obtienen la cantidad de usuarios conectados al directorio.
    ** Si hay 1 solo, no se manda broadcast, pues el unico que hay
    ** es el que hizo la modificacion del archivo en primera instancia.
    */
  int cantUsersInDir = CantUsersLinkToDir(process.dir);
  printf("Tengo %d clientes asociados al directorio %s\n",cantUsersInDir,process.dir);
  fflush(stdout);


   /* Se almacenan en userPidArray los pids de dichos usuarios
   *
   */
   userPidArray = PIDsLinkToDir(process.dir);

  while(status<=ERROR)
  {
    status = InitCommunication(process.pid);
    usleep(__POOL_WAIT__);
  }
  
  printf("Cree la comunicacion con el pid %d y espero por los datos\n",process.pid);
  fflush(stdout);
  
  if(status > ERROR)
  {
  /* Recibo el archivo nuevo */
    while(!requestExists)
    {
      if( (data = GetRequest()) != NULL)
      {
        printf("Proceso lo recibido\n");
        fflush(stdout);
        p = ProcessRequest(&data, &size);
        requestExists=TRUE;
        user = GetPIDToUserName(process.aux_pid);
        printf("Ahora se lo tengo que mandar a todos salvo a %s\n",user);
        
        for(i=0;i<cantUsersInDir;i++){
            /* Si es distinto del pid del usuario que me lo mando */
            if( userPidArray[i] != process.aux_pid ) {
                user = GetPIDToUserName(userPidArray[i]);
                printf("Se lo mando a %s\n",user);                
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
  
  printf("Salio todo re joya\n");
  fflush(stdout);
  
  return status;
}

int
DirBroadcastMsg(process_t process, size_t size, byte * data)
{
  //fopen(process.dir,"w+");
    char a[50];
    int status = OK;
    key_t key;
    int cantUsersInDir, i;
    int * userPidArray;
    string aux;

    /* Se obtienen la cantidad de usuarios conectados al directorio.
    ** Si hay 1 solo, no se manda broadcast, pues el unico que hay
    ** es el que hizo la modificacion del archivo en primera instancia.
    */
    cantUsersInDir = CantUsersLinkToDir("aaa");

    if(cantUsersInDir < 2)
    {
        sprintf(a, "cantidad%d", cantUsersInDir);
      fopen(a, "w+");
      return OK;
    }
    fopen("entrandobroadmod1","w+");
    

    /* Se almacenan en userPidArray los pids de dichos usuarios
    *
    */
    userPidArray = PIDsLinkToDir("aaa");

    if(userPidArray == NULL)
      return ERROR;
    fopen("entrandobroadmod2","w+");
    for(i = 0; i < cantUsersInDir; i++)
    {
        /* Se envia el mensaje a todos, excepto al cliente que
        *  genero el broadcast a traves de una modificacion del directorio.
        */
        if(userPidArray[i] != process.pid)
        {fopen("entrandobroadmod3","w+");
            /* Se genera el key correspondiente al usuario, y se switchea 
            *  la comunicacion a ese canal.
            */
            key = ftok(aux = Concat(bk_path,"aaa"), userPidArray[i]);

            free(aux);
            do
            {
                status = InitCommunication(key);
                usleep(__POOL_WAIT__);
            } while(status <= ERROR);
            
            /* Se envia el mensaje
            */
            status = ProcessSendPack(&data, size);              
        }
    }

    /* Antes de finalizar se vuelve al canal default de comunicación
    */
    key = ftok(aux = Concat(bk_path,process.dir), __DEFAULT_PID__);
    free(aux);
    status = InitCommunication(key);
    
    return OK;
    
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
	























