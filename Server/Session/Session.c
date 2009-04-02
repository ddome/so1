/*
*  Project Includes
*/


#include "../App/Server.h"
#include "CallApp.h"
#include "Session.h"

/*
*  Types
*/

typedef struct{
    int dni;
    int esgay;
} cacaT;

 
/* Static Functions */


static int GetDataSize( session_t data );
static int ProcessCall( session_t *data );
//static byte * MakeSessionData( session_t data );
//static session_t GetSessionData( byte *data );


/* Functions */


int
InitCommunication(pid_t pid)
{
    return InitIPC(pid);
}

byte ** GetRequest(void)
{
    return ReadIPC();
}

int
ProcessRequest(byte ** data, pid_t requestPid)
{
/*	session_t pack;
	process_t ret;
	
	pack  = GetSessionData(*data);
    ret.opCode  = ProcessCall( &pack );
	ret.pid = pack.pid;
	free(*data);
	*data = MakeSessionData(pack);*/
    int ret = 0;
	if((*((cacaT**)data))->dni>0)
	{
	    printf("llego un mensaje!: %d - %s es gay \n", (*((cacaT**)data))->dni,(*((cacaT**)data))->esgay?"SI":"NO");
	    SpawnSubProcess(__SPAWN_DIR__,requestPid,"/");
	}
	else
	    printf("Rock and roll neneeee!\n");
	
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
			return CallFileAdd(*data);
			break;
		
		case PR_EXT:
			return __SHUT_DOWN__;
			break;	
			
		/*	 		 			 
			 case CL_DIR_REQ:			
			 return CallDirReq(data);
			 break;
		 
			case CL_DIR_LST:			
			return CallDirList(data);
			break;
		 
			 
			 case CL_EXT:			
			 return __SHUT_DOWN__
			 break;
			 */	
			
		default:
			return ERROR;
	}	
}

byte *
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






