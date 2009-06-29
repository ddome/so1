/*
*  Project Includes
*/

#include "Session.h"
#include "CallApp.h"
#include "../Transport/OutputPipe.h"

char dir_reg[150][150];
int dir_reg_index=0;

extern char *bk_path;
 
/* Static Functions */


static int GetDataSize( session_t data );
static process_t ProcessCall( session_t *data );
static size_t MakeSessionData( session_t data, byte ** pack );
static session_t GetSessionData( byte *data );
static int MakeFilePack( fileT file, byte *data, byte **dataBuffer );
static int MakeFileRemPack( fileT file, byte **dataBuffer );
/* Functions */

int
InitCommunication(key_t key)
{
   return InitIPC(key);
}

byte * GetRequest(void)
{	
	return ReadIPC();
}

/*
* ProcessRequest: Procesa los pedidos entrantes
*/
process_t
ProcessRequest(byte ** data, size_t * size)
{
	session_t pack;
	process_t process;
		
	pack  = GetSessionData(*data);
	process = ProcessCall( &pack );
	free(*data);
	*size = MakeSessionData(pack,data);

	return process;
}


/*
* ProcessSendPack: Procesa los mensajes de protocolo salientes
*/

int   
ProcessSendPack(byte ** data, size_t size)
{
    return WriteIPC(*data, size);
}


/* Send Functions  */

int 
SendConectionSignal(  pid_t pid )
{
	session_t aux;
	byte * data;
	size_t size;
    
	aux.pid = pid;
	aux.opCode = CL_NEW_CON;
	aux.dataSize = 0;
	
	size = MakeSessionData(aux, &data);
	int status;
	/* Me conecto al servidor */
	do
	{
		status = InitCommunication(__DEFAULT_PID__);
		usleep(__POOL_WAIT__);
	} while(status == ERROR);
	
	return WriteIPC(data, size);
}	

int 
SendDirConectionSignal(  pid_t pid, string dir )
{
	session_t aux;
        byte * data;
        size_t size;
    
	pid_t DirPid = getpid();
	sprintf(aux.senderID, "%d", (int)DirPid);
	aux.pid = pid;
	aux.opCode = CL_DIR_CON;
	aux.dataSize = strlen(dir)+1;
	aux.data = dir;
	size = MakeSessionData(aux, &data);
	
	return WriteIPC(data, size)>0?OK:ERROR;
}	
	
int 
SendNewClientSignal( string userName, pid_t pid )
{
	session_t aux;
	byte * data;
        size_t size;
	
	aux.pid = pid;
	strcpy(aux.msg,userName);
	aux.opCode = CL_NEW_USR;
	aux.dataSize = 0;
	
	size = MakeSessionData(aux, &data);
	
	return WriteIPC(data, size)>0 ? OK: ERROR ;
}

int 
SendFileAddPack( byte * fileData )
{
/*  session_t pack;
  byte *data;
  size_t size;
  fileT file;
  pack = GetSessionData(prevData);
  data = ReqFile(*((fileT*)(pack.data))); 
  pack.opCode = CL_FIL_ADD;
    //strcpy(pack.msg,());
  pack.pid = getppid();
  /* Armo el paquete con la informacion del file a mandar */
//  pack.dataSize = MakeFilePack(*((fileT*)(pack.data)), data, &pack.data );
    
 // size = MakeSessionData(pack, &data);
 // return WriteIPC(data, size)>0?OK:ERROR;*/
  return OK;
}

int 
SendFileModPack( byte * prevData )
{
  
	session_t pack;
	byte *data;
	size_t size;
    fileT file;
    pack = GetSessionData(prevData);
    data = ReqFile(*((fileT*)(pack.data))); 
	pack.opCode = CL_FIL_MOD;
	//strcpy(pack.msg,());
      //  pack.pid = getppid();
	/* Armo el paquete con la informacion del file a mandar */
	pack.dataSize = MakeFilePack(*((fileT*)(pack.data)), data, &pack.data );
	size = MakeSessionData(pack, &data);
	return WriteIPC(data, size)>0?OK:ERROR;
}

int 
SendFileModPacket( session_t pack)
{
    byte * data;
    size_t size;
    size = MakeSessionData(pack, &data);
    return SendFileModPack(data);
}

int
SendFileModTransferSignal( pid_t user_pid, fileT file, pid_t pid)
{
	session_t pack;
	byte *data;
	size_t size;
    char aux_pid[15];
    
    sprintf(aux_pid,"%d",user_pid);
    
    printf("Le voy a mandar %s\n",aux_pid);
    fflush(stdout);
      
    pack.opCode = CL_FIL_MOD_SIGNAL;
    /* Usuario solicitante */
	strcpy(pack.msg,aux_pid);
	/* Aca le voy a mandar la info */
    pack.pid = pid;

    fflush(stdout);

    pack.dataSize = strlen(file.fName)+1;
    pack.data= malloc(strlen(file.fName)+1);
    strcpy(pack.data,file.fName);
    strcpy(pack.senderID,file.path);
    
    printf("Copie el nombre del archivo %s y pesa %d\n",pack.data,pack.dataSize);
    fflush(stdout);

	size = MakeSessionData(pack, &data);
	
	printf("El paquete pesa %d\n",size);
    fflush(stdout);
	return WriteIPC(data, size)>0?OK:ERROR;
}

int
SendFileAddTransferSignal( pid_t user_pid, fileT file, pid_t pid)
{
	session_t pack;
	byte *data;
	size_t size;
    char aux_pid[15];
    
    sprintf(aux_pid,"%d",user_pid);
    
    printf("Le voy a mandar %s\n",aux_pid);
    fflush(stdout);
      
    pack.opCode = CL_FIL_ADD_SIGNAL;
    /* Usuario solicitante */
	strcpy(pack.msg,aux_pid);
	/* Aca le voy a mandar la info */
    pack.pid = pid;

    pack.dataSize = strlen(file.fName)+1;
    pack.data= malloc(strlen(file.fName)+1);
    strcpy(pack.data,file.fName);
    strcpy(pack.senderID,file.path);
    
    printf("Copie el nombre del archivo %s y pesa %d\n",pack.data,pack.dataSize);
    fflush(stdout);

	size = MakeSessionData(pack, &data);
	
	printf("El paquete pesa %d\n",size);
    fflush(stdout);
	return WriteIPC(data, size)>0?OK:ERROR;
}

int
SendFileDelTransferSignal( pid_t user_pid, fileT file, pid_t pid)
{
	session_t pack;
	byte *data;
	size_t size;
    char aux_pid[15];
    
    sprintf(aux_pid,"%d",user_pid);
    
    printf("Le voy a mandar %s\n",aux_pid);
    fflush(stdout);
      
    pack.opCode = CL_FIL_DEL_SIGNAL;
    /* Usuario solicitante */
	strcpy(pack.msg,aux_pid);
	/* Aca le voy a mandar la info */
    pack.pid = pid;

    pack.dataSize = sizeof(fileT);
    pack.data= malloc(sizeof(fileT));
    *(fileT *)(pack.data) = file;
    strcpy(pack.senderID,file.path);
        
    printf("Copie el nombre del archivo %s en el dir %s y pesa %d\n",file.fName,pack.senderID,pack.dataSize);
    fflush(stdout);

	size = MakeSessionData(pack, &data);
	
	printf("El paquete pesa %d\n",size);
    fflush(stdout);
	return WriteIPC(data, size)>0?OK:ERROR;
}

int 
SendFile( fileT file, pid_t pid )
{

    printf("Entre a la funcion\n");
    fflush(stdout);

   	session_t pack;
	byte *data;
	size_t size;

	pack.opCode = CL_FIL_TRAN;
    pack.pid = pid;
    
    data = FileReq(file);

    fflush(stdout);

	/* Armo el paquete con la informacion del file a mandar */
	pack.dataSize = MakeFilePack(file, data, &pack.data );
	
	strcpy(pack.senderID,Concat(file.path,Concat("/",file.fName)));

	size = MakeSessionData(pack, &data);
	
	printf("Le voy a mandar el archivo --%s-- que pesa %d\n",pack.senderID,GetSize(file));
	
	return WriteIPC(data, size)>0?OK:ERROR; 

}

int
SendDirRemoveSignal( string userName, string dir, pid_t pid )
{
  session_t pack;
  byte *data;
  size_t size;
   
  fflush(stdout); 
    
  pack.pid = pid;
  pack.opCode = CL_DIR_REM;           
  strcpy(pack.msg,userName);
  pack.dataSize = strlen(dir)+1;
  pack.data = malloc(strlen(dir)+1);
  strcpy(pack.data,dir); 
  
  printf("%s %s %d\n",pack.data, pack.msg, pack.dataSize);
  fflush(stdout);
         
  size = MakeSessionData(pack, &data);
  return WriteIPC(data, size)>0?OK:ERROR;   
}

int 
SendFileRemPack( string userName, fileT file, pid_t pid )
{
	session_t pack;
	byte *data;
	size_t size;
   	pack.pid = pid;
	pack.opCode = CL_FIL_REM;
	strcpy(pack.msg,userName);
	pack.dataSize = MakeFileRemPack(file, &pack.data);

	size = MakeSessionData(pack, &data);
	return WriteIPC(data, size)>0?OK:ERROR;
}

int 
SendDirReq( string userName, pid_t pid, string dirPath )
{
	session_t pack;
	byte *data;
	size_t size;
		
	pack.opCode = CL_DIR_REQ;
	strcpy(pack.msg,userName);

	pack.dataSize = strlen(dirPath)+1;
	pack.data = dirPath;
	pack.pid = pid;

	size = MakeSessionData(pack, &data);
	
	strcpy(dir_reg[dir_reg_index++],dirPath);
	
	return WriteIPC(data, size)>0?OK:ERROR;
}	

int
SendDirListReq( string userName )
{
	session_t pack;
	byte *data;
	size_t size;
	pack.pid = getppid();
	pack.opCode = CL_DIR_LST;
	strcpy(pack.msg,userName);
	pack.dataSize = 0;
	
	size = MakeSessionData(pack, &data);

	return WriteIPC(data, size)?OK:ERROR;
}

int 
SendExitSignal( string userName )
{
	session_t pack;
	size_t size;
	byte *data;
	pack.pid = 0;
	pack.opCode = CL_EXT;
	strcpy(pack.msg,userName);
	pack.dataSize = 0;	
	
	size = MakeSessionData(pack, &data);
	pack = GetSessionData(data);
	
	kill(getppid(),SIGINT);

    int i;
    for(i=0;i<dir_reg_index;i++){
        
        KillInotify(getppid(),dir_reg[i]);
        
    }
	
	return WriteIPC(data, size);
}

int
SendDirDel( string path, string dirName, pid_t pid )
{
	char * serverPath;
    	key_t keyDir;
    	session_t pack;
	size_t size;
	byte *data;
    	int status;
    
	pack.opCode = CL_DIR_DEL;
	strcpy(pack.msg,dirName);
	strcpy(pack.senderID,path);
	pack.dataSize = 0;	
	pack.data = NULL;
	pack.pid = pid;
	
	size = MakeSessionData(pack, &data);
	pack = GetSessionData(data);
	
	InitIPC(__DEFAULT_PID__);
	status =  WriteIPC(data, size)?OK:ERROR;
	return status;	
}

int
SendDirNew( string path, string dirName, pid_t pid )
{
	char * serverPath;
    	key_t keyDir;
    	session_t pack;
	size_t size;
	byte *data;
    	int status;
    
	pack.opCode = CL_DIR_NEW;
	strcpy(pack.msg,dirName);
	strcpy(pack.senderID,path);
	pack.dataSize = 0;	
	pack.data = NULL;
	pack.pid = pid;
	
	size = MakeSessionData(pack, &data);
	pack = GetSessionData(data);
	
	InitIPC(__DEFAULT_PID__);
	status =  WriteIPC(data, size)?OK:ERROR;
	return status;	
}

/* Static Functions */

static int
GetDataSize( session_t data )
{
	return ( MAXSENDER + MAXMSG + sizeof(uInt) + sizeof(pid_t) 
			+ sizeof(size_t) + data.dataSize );
}

static process_t
ProcessCall( session_t *data )
{	
    int status;
	process_t p;
    char * aux;
	switch ((*data).opCode) {
		case SR_CONECT_OK:
			printf("Se ha conectado exitosamente\n");
			fflush(stdout);
			p.opCode = __NO_RESPONSE__;
			p.status = OK;
			break;
		case SR_NEW_USR_OK:
			printf("Ha registrado el nombre de usuario exitosamente\n");
			fflush(stdout);
			p.opCode = __NO_RESPONSE__;
			p.status = OK;
			break;
		case SR_NEW_USR_ERR:
			printf("El nombre usuario no se encuentra disponible o usted ya se encuentra conectado.\n");
			fflush(stdout);
			p.opCode = __NO_RESPONSE__;
			p.status = OK;
			break;
		case SR_FIL_REM:
			printf("Recibi la borrada!!!! :)\n");
		    fflush(stdout);
		    /* Le aviso al inotify que no avise mientras se
		    *  actualizan las carpetas */
		    do
		    {
		        status = InitINotifyMsg(getpid(),data->senderID);
		        usleep(__POOL_WAIT__);
		    } while (status != OK);
		    
		    status = WriteINotifyMsg(__INOTIFY_DISABLE__);
			p.status = CallFileRem(*data);
			
			sleep(3);
			
		    /* Ya termine de modificar, vuelvo a habilitar
		    *  el inotify */
		    status = WriteINotifyMsg(__INOTIFY_ENABLE__);
		    
		    p.opCode = __NO_RESPONSE__;
			break;
		case SR_EXT:
			p.opCode = __SHUT_DOWN__;
			p.status = OK;
			break;
		case SR_DIR_LST:
			p.status = CallDirList(*data);
			p.opCode = __NO_RESPONSE__;
			break;
		case SR_DIR_REQ_OK:
		    fflush(stdout);
			p.status = OK;
			p.opCode = __SPAWN_DEMAND__;
			/* Directorio */
			strcpy(p.dir, (*data).data);
			/* Pid en el que recibiria */
			p.pid = (*data).pid;
			break;			
		case SR_FIL_TRAN_REQ:
		    printf("Recibi un pedido para iniciar transferencia\n");
		    fflush(stdout);
			p.status = OK;
			p.opCode = __SPAWN_SND_DEMAND__;
			/* Directorio */
			strcpy(p.dir, data->senderID);
			/* Pid en el que recibiria */
			p.pid = (*data).pid;
			
			printf("El nombre del directorio seria %s y el pid por el cual escuchar\n",p.dir,p.pid);
		    fflush(stdout);
			
			break;
	    case SR_FIL_TRAN:
	        printf("Recibi el archivo!!! :)\n");
		    fflush(stdout);
			p.status = OK;
			p.opCode =  __NO_RESPONSE__;
			strcpy(p.dir,data->senderID);
			CallFileTransfer(*data);			
			break;
			
		case SR_DIR_TRANS:
		    fflush(stdout);
			p.status = CallDirAdd(*data);
			p.opCode = __NO_RESPONSE__;
			break;
			
		case SR_DIR_REM:
		
				 printf("Borro un dir");
                 fflush(stdout);	
			do
		    {
		        status = InitINotifyMsg(getpid(),data->data);
		        usleep(__POOL_WAIT__);
		    } while (status != OK);
		    
		    WriteINotifyMsg(__INOTIFY_DISABLE__); 
		    
			p.status = CallDirDel(*data);
			p.opCode = __NO_RESPONSE__;
			
			sleep(2);
    
            WriteINotifyMsg(__INOTIFY_ENABLE__);
            
			break;	
		case SR_DIR_NEW:
		
		        printf("Creo un dir");
                 fflush(stdout);
		    
			do
		    {
		        status = InitINotifyMsg(getpid(),data->data);
		        usleep(__POOL_WAIT__);
		    } while (status != OK);
		    
		    WriteINotifyMsg(__INOTIFY_DISABLE__);
		
			p.status = CallDirNew(*data);
			p.opCode = __NO_RESPONSE__;
			
			sleep(2);
			
			WriteINotifyMsg(__INOTIFY_ENABLE__);
			
			break;
			
			
		default:
			p.status = ERROR;
			break;
	}
		
	return p;
}

static size_t
MakeSessionData( session_t data, byte **dataBuffer )
{
	byte *aux;
	uInt pos;

	if( (aux=malloc(sizeof(byte)*GetDataSize(data))) == NULL ) {
		return ERROR;
	}
	
	pos = 0;
	memmove(aux+pos, data.senderID, MAXSENDER);
	pos += MAXSENDER;
	memmove(aux+pos, data.msg, MAXMSG);
	pos += MAXMSG;
	memmove(aux+pos, &(data.opCode), sizeof(uInt) );
	pos += sizeof(uInt);
	memmove(aux+pos, &(data.pid), sizeof(pid_t) );
	pos += sizeof(pid_t);
	memmove(aux+pos, &(data.dataSize), sizeof(size_t) );
	pos += sizeof(size_t);
    
	memmove(aux+pos, data.data, data.dataSize );
    pos += data.dataSize;
    
    *dataBuffer = aux;
    
	return pos;
}

static session_t
GetSessionData( byte *data )
{
	int pos;
	session_t aux;
	
	pos = 0;
	memmove(aux.senderID, data+pos, MAXSENDER);
	pos += MAXSENDER;	
	
	memmove(aux.msg, data+pos, MAXMSG);
	pos += MAXMSG;
	
	memmove(&(aux.opCode), data+pos, sizeof(uInt) );
	pos += sizeof(uInt);
	
	memmove(&(aux.pid), data+pos, sizeof(pid_t) );
	pos += sizeof(pid_t);
	
	memmove(&(aux.dataSize), data+pos, sizeof(size_t) );
	pos += sizeof(size_t);
	
	/* Reservo memoria para el data */
	if( aux.dataSize > 0 ) {
		aux.data = malloc(aux.dataSize);
		memmove(aux.data, data+pos, aux.dataSize );
	}
	else {
		aux.data = NULL;
	}
	
	return aux;
}

static int
MakeFilePack( fileT file, byte *data, byte **dataBuffer )
{
	if( (*dataBuffer=malloc(GetSize(file)+sizeof(fileT))) == NULL ) {
		return ERROR;
	}
	
	memmove(*dataBuffer, &file, sizeof(fileT));
	if( data != NULL ) {
		memmove(*dataBuffer+sizeof(fileT), data, GetSize(file));
	}
	
	return (GetSize(file) + sizeof(fileT));
}


static int
MakeFileRemPack( fileT file, byte **dataBuffer )
{
	if( (*dataBuffer=malloc(sizeof(fileT))) == NULL ) {
		return ERROR;
	}
	
	memmove(*dataBuffer, &file, sizeof(fileT));
	
	return (sizeof(fileT));
}

/* funciones de debugueo */


static int
GetFileListSize( int nfiles, fileT *fileList )
{
	int size;
	int i;
	
	size = 0;
	for( i=0; i<nfiles; i++ ) {
		size += GetSize(fileList[i]);
	}
	
	return size;
}



static int
MakeDirPack(int nfiles, fileT * fileList,byte **dataBuffer,byte **pack)
{
	int size;
	int i;
	int pos;
	
	if( (*pack = malloc(size=sizeof(int)+sizeof(fileT)*nfiles+GetFileListSize(nfiles,fileList)) ) == NULL ) {
		return ERROR;
	}
	
	pos = 0;
	memmove(*pack + pos, &nfiles, sizeof(int));	
	pos += sizeof(int);
	
	for( i=0; i<nfiles; i++ ) {		
		memmove(*pack + pos, &(fileList[i]), sizeof(fileT));
		pos += sizeof(fileT);
		memmove(*pack + pos, dataBuffer[i], GetSize(fileList[i]));
		pos += GetSize(fileList[i]);
	}
	
	return size;	
}	

int
CallTransferDir(session_t * dataPtr)
{
	byte **dataBuffer;
	fileT *fileList;
	string dirPath;
	string userName;
	int nfiles;
    
	dirPath  = (*dataPtr).data;
	userName = (*dataPtr).msg;
	
	/* armo el paquete respuesta */	
	(*dataPtr).opCode = SR_DIR_TRANS;
	
	if( (nfiles=ReqDir(userName, dirPath, &fileList, &dataBuffer)) == ERROR ) {
		return ERROR;
	}
	else {
		if( ((*dataPtr).dataSize = MakeDirPack(nfiles, fileList,dataBuffer,&((*dataPtr).data))) != ERROR ) {
			return OK;
		}
		else {
			return ERROR;
		}
	}
}

int
SendDirPack(process_t process)
{
    session_t session;
    size_t size;
    byte * data;
	
    char * userName = CreateString("damian");
	
    if(userName != NULL)
    {
		strcpy(session.msg, userName);
		free(userName);
    }
	
	session.dataSize = strlen("in") + 1;
	session.data = malloc(session.dataSize);
    strcpy(session.data, "in");
	
    CallTransferDir(&session);
    
    size = MakeSessionData(session, &data);
    ProcessRequest(&data, &size);
	
	return OK;
}





