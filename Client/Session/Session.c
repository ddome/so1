/*
*  Project Includes
*/

#include "Session.h"
#include "CallApp.h"


 
/* Static Functions */


static int GetDataSize( session_t data );
static int ProcessCall( session_t *data );
static byte * MakeSessionData( session_t data );
static session_t GetSessionData( byte *data );


/* Functions */


int
InitCommunication(pid_t pid)
{
   //return InitIPC(pid);
}

pid_t GetRequest(void * data)
{
     //return ReadIPC(data);
}

int
ProcessRequest(byte ** data, pid_t requestPid)
{
	session_t pack;
	int ret;
			
	pack  = GetSessionData(*data);
    ret = ProcessCall( &pack );
	
	free(*data);
	*data = MakeSessionData(pack);

	/*if(((cacaT*)data)->dni>0)
	{
	    printf("llego un mensaje!: %d - %s es gay \n", ((cacaT*)data)->dni,((cacaT*)data)->esgay?"SI":"NO");
	    SpawnSubProcess(__SPAWN_DIR__,requestPid,"/");
	}
	else
	    printf("Rock and roll neneeee!\n");*/
	
	
	return ret;
}

/* Send Functions */

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
SendFileAddPack( string userName, fileT file, byte *data )
{
	session_t pack;
	
	pack.opCode = CL_FIL_ADD;
	strcpy(pack.msg,userName);
	pack.dataSize = MakeFilePack( file, data, &pack.data );
	
	return OK; //LLAMARTRANSPORTE( MakeSessionData(pack) );
	
}

int 
SendFileModPack( string userName, fileT file, byte *data )
{
	session_t pack;
	
	pack.opCode = CL_FIL_MOD;
	strcpy(pack.msg,userName);
	pack.dataSize = MakeFilePack( file, data, &pack.data );
	
	return OK; //LLAMARTRANSPORTE( MakeSessionData(pack) );
}

int 
SendFileRemPack( string userName, fileT file )
{
	session_t pack;
	
	pack.opCode = CL_FIL_REM;
	strcpy(pack.msg,userName);
	pack.dataSize = MakeFilePack( file, NULL, &pack.data );
	
	return OK; //LLAMARTRANSPORTE( MakeSessionData(pack) );
}

int 
SendDirReq( string userName, string dirPath, fileT **fileList, byte ***dataBuffer )
{
	session_t pack;
	
	pack.opCode = CL_DIR_REQ;
	strcpy(pack.msg,userName);
	pack.dataSize = 0;
	pack.data = NULL;
	
	return OK; //CallAddDir( LLAMARTRANSPORTE( MakeSessionData(pack) );)
}	

int
SendDirListReq( string userName )
{
	session_t pack;
	
	pack.opCode = CL_DIR_LST;
	strcpy(pack.msg,userName);
	pack.dataSize = 0;
	pack.data = NULL;
	
	return OK; // string = GetString( LLAMARTRANSPORTE( MakeSessionData(pack) );)	
}

int 
SendExitSignal( string userName )
{
	session_t pack;
	
	pack.opCode = CL_EXT;
	strcpy(pack.msg,userName);
	pack.dataSize = 0;
	pack.data = NULL;	

	return OK;
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
	switch ((*data).opCode) {
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

static byte *
MakeSessionData( session_t data )
{
	byte *aux;
	uInt pos;
	
	if( (aux=malloc(sizeof(byte)*GetDataSize(data))) == NULL ) {
		return NULL;
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
	
	return aux;
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




