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

/* New Client */

static int
MakeNewClientPack(int op, session_t *dataPtr )
{
	session_t aux;
	int ret;
	
	/* Armo el paquete de respuesta */
	aux.msg[0]      = '\0';
	aux.senderID[0] = '\0';
	aux.pid			= 0;
	aux.dataSize    = 0;
	aux.data		= NULL;
	
	switch(op) {
		case NEW_USR_EXIST: 
			aux.opCode = SR_NEW_USR_ERR;
			ret = OK;
			break;
			
		case NEW_USR_OK:
			aux.opCode = SR_NEW_USR_OK;
			ret = OK;
			break;
			
		default:
			aux.opCode = ERROR;
			ret = ERROR;
	}
	
	//Guardo la respuesta del Server
    *dataPtr = aux;
	return ret;		
}

static int 
CallNewClient(session_t *dataPtr)
{	
	int ret;
	
	ret=NewClient((*dataPtr).msg,(*dataPtr).pid);	
	return (MakeNewClientPack(ret, dataPtr));
}	

/* UserList */

static byte *
MakeMemBlockUsrList(string *usrLst, int nusr)
{
	byte *memblock;
	int pos;
	int i;
	
	/* Almaceno un bloque continuo de memoria para guardar todos los arreglos */
	if( (memblock = malloc(sizeof(string) * (MAX_USR_NAME+1) * nusr )) == NULL ) {
		return NULL;
	}
	
	/*Copio uno por uno los strings, de manera continua sobre el bloque */
	pos = 0;
	for( i=0; i < nusr; i++ ) {
		memmove(memblock + pos, usrLst[i], strlen(usrLst[i])+1 );
		pos += MAX_USR_NAME+1;
	}
	
	return memblock;
}
	
static int
MakeUserListPack(string *usrLst, int nusr, session_t *dataPtr)
{
	session_t aux;
	
	aux.opCode = (*dataPtr).opCode;
	aux.msg[0]      = '\0';
	aux.senderID[0] = '\0';
	aux.pid			= 0;
	aux.dataSize    = nusr*sizeof(string)*(MAX_USR_NAME+1);
	if(  (aux.data = MakeMemBlockUsrList(usrLst, nusr)) == NULL ) {
		return ERROR;
	}
	*dataPtr = aux;	
	return OK;
}
	
static int 
CallUserList(session_t *dataPtr)
{
	string *usrLst;
	int nusr;
	
	if( (nusr=UserList(&usrLst)) == ERROR ) {
		return ERROR;
	}
	
	return MakeUserListPack(usrLst, nusr, dataPtr);
}	
	

/*
static int CallTopList(session_t data);
static int CallTopListUser(session_t data);
static int CallDirReq(session_t data);
static int CallDirRem(session_t data);
static int CallFileAdd(session_t data);
static int CallFileRem(session_t data);
static int CallFileAdd(session_t data);
static int CallDirList(session_t data);
*/


int
ProcessCall( session_t *data )
{

	switch( (*data).opCode ) {
		case PR_DIR_REG:
			return CallNewClient(data);
			break;
			
		case PR_USR_LST:
			return CallUserList(data);
			break;
			
/*		case PR_ACT_LST:			
			return CallTopList(data);
			break;
			
		case PR_ACT_USR:
			return CallTopListUser(data);
			break;	
			
		case PR_EXT:
			return __SHUT_DOWN__;
			break;
			
		case CL_DIR_REQ:			
			return CallDirReq(data);
			break;
			
		case CL_DIR_REM:			
			return CallDirRem(data);
			break;
			
		case CL_FIL_ADD:			
			return CallFileAdd(data);
			break;	
			
		case CL_FIL_REM:			
			return CallFileRem(data);
			break;
			
		case CL_FIL_MOD:			
			return CallFileAdd(data);
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
ProcessRequest(void ** data, pid_t requestPid)
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
	
	//return (MakeSessionData(outPack)); Deberia devolver esto
	return ret;
}

void
GoodBye(void)
{
    
}

void ShutDown(void)
{
    
}





