/*
*  Project Includes
*/

#include "Session.h"
#include "CallApp.h"


 
/* Static Functions */


static int GetDataSize( session_t data );
static int ProcessCall( session_t *data );
static size_t MakeSessionData( session_t data, byte ** pack );
static session_t GetSessionData( byte *data );
static int MakeFilePack( fileT file, byte *data, byte **dataBuffer );
static int GetDirList( session_t pack, string **out );

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

int
ProcessRequest(byte ** data, pid_t requestPid)
{
	session_t pack;
	int ret;
		
	pack  = GetSessionData(*data);
        ret = ProcessCall( &pack );
	
	free(*data);
	MakeSessionData(pack,data);

	return ret;
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
SendDirReq( string userName, string dirPath )
{
	session_t pack;
	byte *data;
	
	pack.opCode = CL_DIR_REQ;
	strcpy(pack.msg,userName);
	pack.dataSize = 0;
	pack.data = NULL;
	
	MakeSessionData(pack, &data);
	
	return OK; //LLAMARTRANSPORTE( MakeSessionData(pack) );
}	

int
SendDirListReq( string userName )
{
	session_t pack;
	byte *data;
	
	pack.opCode = CL_DIR_LST;
	strcpy(pack.msg,userName);
	pack.dataSize = 0;
	pack.data = NULL;
	
	MakeSessionData(pack, &data);
	
	return OK; // Llamar transporte	
}

int 
SendExitSignal( string userName )
{
	session_t pack;
        size_t size;
	byte *data;
	
	pack.opCode = CL_EXT;
	strcpy(pack.msg,userName);
	pack.dataSize = 0;
	pack.data = NULL;	
	
        size = MakeSessionData(pack, &data);
	
	return WriteIPC(data, size);
}


/* Static Functions */

static int
GetDataSize( session_t data )
{  
	return ( MAXSENDER + MAXMSG + sizeof(uInt) + sizeof(pid_t) 
			+ sizeof(size_t) + data.dataSize );
}

static int
ProcessCall( session_t *data )
{	
	switch ((*data).opCode) {
		case SR_CONECT_OK:
			//funcion que imprime ok en prompt
			return OK;
			break;
		case SR_NEW_USR_OK:
			//funcion que imprime ok en prompt
			return OK;
			break;
		case SR_NEW_USR_ERR:
			//funcion que avisa que el nombre ya existe
			return OK;
			break;
		case SR_DIR_ADD:
			return CallDirAdd(*data);
			break;			
		case SR_FIL_ADD:
			return CallFileAdd(*data);
			break;
		case SR_FIL_MOD:
			return CallFileMod(*data);
			break;
		case SR_FIL_REM:
			return CallFileRem(*data);
			break;
		case SR_EXT:
			return __SHUT_DOWN__;
			break;
	
		default:
			return ERROR;
			break;
	}
		
	return OK;
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
GetDirList( session_t pack, string **out )
{
	int i;
    int ndirs;
	int pos;
	
	
	if( (*out=malloc(sizeof(char)*MAX_DIR_NAME)) == NULL ) {
		return ERROR;
	}
	
	pos = 0;
	memmove(&ndirs, pack.data, sizeof(int));
	pos += sizeof(int);
	
	for( i=0; i<ndirs; i++ ) {
		memmove(&((*out)[i]), pack.data, MAX_DIR_NAME );
		pos += MAX_DIR_NAME;
	}
	
	return ndirs;	
}	