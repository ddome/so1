/*
*  Project Includes
*/

#include "Session.h"
#include "CallApp.h"
#include "../Transport/OutputPipe.h"


 
/* Static Functions */


static int GetDataSize( session_t data );
static process_t ProcessCall( session_t *data );
static size_t MakeSessionData( session_t data, byte ** pack );
static session_t GetSessionData( byte *data );
static int MakeFilePack( fileT file, byte *data, byte **dataBuffer );

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
	
	return WriteIPC(data, size);
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
	
	return WriteIPC(data, size);
}

int 
SendFileAddPack( string userName, fileT file, byte *dataBuffer )
{
	session_t pack;
	byte *data;
	
	pack.opCode = CL_FIL_ADD;
	strcpy(pack.msg,userName);
	pack.dataSize = MakeFilePack( file, dataBuffer, &pack.data );
	
	MakeSessionData(pack, &data);
	
	return OK; //LLAMARTRANSPORTE( MakeSessionData(pack) );
	
}

int 
SendFileModPack( string userName, fileT file, byte *dataBuffer )
{
	session_t pack;
	byte *data;
	
	pack.opCode = CL_FIL_MOD;
	strcpy(pack.msg,userName);
	pack.dataSize = MakeFilePack( file, dataBuffer, &pack.data );
	
	MakeSessionData(pack, &data);
	
	return OK; //LLAMARTRANSPORTE( MakeSessionData(pack) );
}

int 
SendFileRemPack( string userName, fileT file )
{
	session_t pack;
	byte *data;
	
	pack.opCode = CL_FIL_REM;
	strcpy(pack.msg,userName);
	pack.dataSize = MakeFilePack( file, NULL, &pack.data );

	MakeSessionData(pack, &data);
	
	return OK; //LLAMARTRANSPORTE( MakeSessionData(pack) );
}

int 
SendDirReq( string userName, pid_t pid, string dirPath )
{
	session_t pack;
	byte *data;
	size_t size;
	
	pack.opCode = CL_DIR_REQ;
	strcpy(pack.msg,userName);

	pack.dataSize = strlen(dirPath)+1;;	
	pack.data = dirPath;
	pack.pid = pid;

	size = MakeSessionData(pack, &data);
	return WriteIPC(data, size);
}	

int
SendDirListReq( string userName )
{
	session_t pack;
	byte *data;
	size_t size;
	
	pack.opCode = CL_DIR_LST;
	strcpy(pack.msg,userName);
	pack.dataSize = 0;
	
	size = MakeSessionData(pack, &data);

	return WriteIPC(data, size);
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
	
	return WriteIPC(data, size);
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
	process_t p;
	switch ((*data).opCode) {
		case SR_CONECT_OK:
			WritePrompt("Se ha conectado exitosamente\n");
			p.opCode = __NO_RESPONSE__;
			p.status = OK;
			break;
		case SR_NEW_USR_OK:
			WritePrompt("Ha registrado el nombre de usuario exitosamente\n");
			p.opCode = __NO_RESPONSE__;
			p.status = OK;
			break;
		case SR_NEW_USR_ERR:
			WritePrompt("El nombre usuario no se encuentra disponible\n");
			p.opCode = __NO_RESPONSE__;
			p.status = OK;
			break;
	/*	case SR_DIR_ADD:
			p.status = CallDirAdd(*data);
			p.opCode = __NO_RESPONSE__;
			break;			*/
		case SR_FIL_ADD:
			p.status = CallFileAdd(*data);
			p.opCode = __NO_RESPONSE__;
			break;
		case SR_FIL_MOD:
			p.status = CallFileMod(*data);
			p.opCode = __NO_RESPONSE__;
			break;
		case SR_FIL_REM:
			p.status = CallFileRem(*data);
			p.opCode = __NO_RESPONSE__;
			break;
		case SR_EXT:
			p.opCode = __SHUT_DOWN__;
			p.status = OK;
			break;
		case CL_DIR_LST:
			p.status = CallDirList(*data);
			p.opCode = __NO_RESPONSE__;
			break;
		case SR_DIR_REQ_OK:
			p.status = OK;
			p.opCode = __SPAWN_DIR__;
			strcpy(p.dir, (*data).data);

			break;
		case SR_DIR_CON_OK:
			p.status = OK;
			strcpy(p.dir, (*data).data);
			p.opCode = __SPAWN_DEMAND__;
			break;
		case SR_DIR_TRANS:
			p.status = CallDirAdd(*data);
			p.opCode = __NO_RESPONSE__;
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



