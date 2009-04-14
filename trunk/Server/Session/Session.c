/*
*  Project Includes
*/


#include "../App/Server.h"
#include "CallApp.h"
#include "Session.h"
 
/* Static Functions */


static int GetDataSize( session_t data );
static process_t ProcessCall( session_t *data );
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
	*size = MakeSessionData(pack, data);
	//ret = WriteIPC(*data, size);
	return process;
}

/*
* ProcessSendPack: Procesa los mensajes salientes
*/

int   
ProcessSendPack(byte ** data, size_t size)
{
    return WriteIPC(*data, size);
}

int
SendDirPack(process_t process)
{
    session_t session;
    size_t size;
    byte ** data;

    char * userName = ConvertPIDToUserName(process.pid);
        
    if(userName != NULL)
    {
      strcpy(session.msg, userName);
      free(userName);
    }
    session.opCode = SR_DIR_TRANS;
    strcpy(session.data, process.dir);
    session.dataSize = strlen(process.dir ) + 1;

    CallTransferDir(&session);
    
    size = MakeSessionData(session, data);


    
    return ProcessSendPack(data, size);
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

static process_t
ProcessCall( session_t *data )
{
        process_t p;
	
	switch( (*data).opCode ) {
			
		case CL_NEW_CON:
			p.status = CallNewConection(data);
			p.opCode = __NOT_SPAWN__;
			break;
			
		case CL_NEW_USR:
			p.status = CallNewClient(data);
            p.opCode = __NOT_SPAWN__;
			break;

		case CL_DIR_REM:			
			p.status = CallDirRem(*data);
            p.opCode = __NOT_SPAWN__;
			break;
			
		case CL_FIL_ADD:			
			p.status = CallFileAdd(*data);
            p.opCode = __NOT_SPAWN__;
			break;
			
		case CL_FIL_REM:			
			p.status = CallFileRem(*data);
            p.opCode = __NOT_SPAWN__;
			break;
			
		case CL_FIL_MOD:			
			p.status = CallFileMod(*data);
            p.opCode = __NOT_SPAWN__;
			break;
			
		case CL_EXT:	
			p.status = CallClientExit(*data);
			p.opCode = __NOT_SPAWN__;
			break;	
		
		case PR_EXT:
			p.status = __SHUT_DOWN__;
			p.opCode = __NOT_SPAWN__;
			break;	
		
		case CL_DIR_REQ:			
			p.status = CallDirReq(data);
			p.opCode = (p.status == 0) ? __SPAWN_DIR__ : __NOT_SPAWN__ ;
            strcpy(p.dir, (*data).data);
            p.pid = (*data).pid;
			break;	
										
		case CL_DIR_LST:			
			p.status = CallDirList(data);
			p.opCode = __NOT_SPAWN__;
			break;
        case CL_DIR_CON:
            (*data).opCode = SR_DIR_CON_OK;
            p.opCode = __SPAWN_DEMAND__;  
            p.pid = (*data).pid; 
            sscanf((*data).senderID, "%d", &(p.status));        
            strcpy(p.dir, (*data).data);
            break;
            
		default:
			p.status = ERROR;
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

/*
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
	
}*/

