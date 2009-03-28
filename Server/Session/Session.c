/*
*  Project Includes
*/

#include "Session.h"

/*
*  Types
*/

typedef struct{
    int dni;
    int esgay;
} cacaT;

/* 
*  Functions
*/

static int
GetDataSize( session_t data )
{
	return ( MAXSENDER + MAXMSG + sizeof(uInt) + sizeof(pid_t) 
			+ sizeof(size_t) + data.dataSize );
}

static int
ProcessCall( uInt opCode, session_t data )
{

	switch( opCode ) {
		case PR_DIR_REG:
			if( (NewClient(data.msg,data.pid)) == ERROR ) {
				// MANDO AVISO DE QUE YA ESTA CONECTADO
			}
			break;
			
		case PR_USR_LST:			
			break;
			
		case PR_ACT_LST:			
			break;
			
		case PR_DIR_USR:			
			break;	
			
		case PR_EXT:			
			break;
			
		case SR_FIL_ADD:			
			break;
		case SR_FIL_REM:			
			printf("No andó :(\n");
			break;
		case SR_FIL_MOD:			
			printf("No andó :(\n");
			break;
		case SR_EXT:			
			printf("No andó :(\n");
			break;
			
		case CL_DIR_REQ:			
			printf("No andó :(\n");
			break;
		case CL_DIR_REM:			
			printf("No andó :(\n");
			break;
		case CL_FIL_ADD:			
			printf("No andó :(\n");
			break;	
		case CL_FIL_REM:			
			printf("No andó :(\n");
			break;		
		case CL_FIL_MOD:			
			printf("No andó :(\n");
			break;	
		case CL_DIR_LST:			
			printf("No andó :(\n");
			break;
		case CL_EXT:			
			printf("No andó :(\n");
			break;
		default:
			return ERROR;
	}
	
	return OK;
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
	aux.data = malloc(aux.dataSize);
	memmove(aux.data, data+pos, aux.dataSize );
	
	return aux;
}

int
InitCommunication(pid_t pid)
{
    return InitIPC(pid);
}

pid_t GetRequest(void * data)
{
    return ReadIPC(data);
}

int
ProcessRequest(void * data, pid_t requestPid)
{
	session_t pack;
	
	//printf("llego un mensaje!: %d - %s es gay \n", ((cacaT*)data)->dni,((cacaT*)data)->esgay?"SI":"NO");
	
	pack = GetSessionData(data);
	ProcessCall( pack.opCode, pack );

	/*if(((cacaT*)data)->dni>0)
	{
	    printf("llego un mensaje!: %d - %s es gay \n", ((cacaT*)data)->dni,((cacaT*)data)->esgay?"SI":"NO");
	    SpawnSubProcess(__SPAWN_DIR__,requestPid,"/");
	}
	else
	    printf("Rock and roll neneeee!\n");*/
	return OK;
}

void
GoodBye(void)
{
    
}

void ShutDown(void)
{
    
}





