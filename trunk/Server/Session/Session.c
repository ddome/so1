/*
*  Project Includes
*/


#include "../App/Server.h"
#include "CallApp.h"
#include "Session.h"
 
/* Static Functions */


static int GetDataSize( session_t data );
static int ProcessCall( session_t *data );
static size_t MakeSessionData( session_t data, byte **pack );
static session_t GetSessionData( byte *data );


/* Functions */


int
InitCommunication(pid_t pid)
{
    return InitIPC(pid);
}

byte * GetRequest(void)
{
    return ReadIPC();
}

int
ProcessRequest(byte ** data, pid_t requestPid)
{
	session_t pack;
	size_t size;
	int ret;
	
	pack  = GetSessionData(*data);
        ret   = ProcessCall( &pack );
	printf("llego un mensaje, opcode:%d", pack.opCode);
	free(*data);
	size = MakeSessionData(pack, data);
	//ret = WriteIPC(*data, size);
	return ret;
}

void
GoodBye(void)
{
    
}

void ShutDown(void)
{
    
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
	
	switch( (*data).opCode ) {
			
		case CL_NEW_CON:
			return CallNewConection(data);
			break;
			
		case CL_NEW_USR:
			return CallNewClient(data);
			break;

		case CL_DIR_REM:			
			return CallDirRem(*data);
			break;
			
		case CL_FIL_ADD:			
			return CallFileAdd(*data);
			break;
			
		case CL_FIL_REM:			
			return CallFileRem(*data);
			break;
			
		case CL_FIL_MOD:			
			return CallFileMod(*data);
			break;
			
		case CL_EXT:			
			return CallClientExit(*data);
			break;	
		
		case PR_EXT:
			return __SHUT_DOWN__;
			break;	
		
		case CL_DIR_REQ:			
			return CallDirReq(data);
			break;	
										
		case CL_DIR_LST:			
			return CallDirList(data);
			break;
			
		default:
			return ERROR;
	}	
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

session_t
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

/* DEBUGUEO */


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
	//WriteIPC(data, size);
	
	ProcessRequest(&data,pid);
	
	return OK;
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

int 
SendFileAddPack( string userName, fileT file, byte *dataBuffer )
{
	session_t pack;
	byte *data;
	
	pack.opCode = CL_FIL_ADD;
	strcpy(pack.msg,userName);
	pack.dataSize = MakeFilePack( file, dataBuffer, &pack.data );
	
	MakeSessionData(pack, &data);
	ProcessRequest(&data,55);
	
	return OK; //LLAMARTRANSPORTE( MakeSessionData(pack) );
	
}

