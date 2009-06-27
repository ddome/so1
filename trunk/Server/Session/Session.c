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
	return process;
}

/*
* ProcessSendPack: Procesa los mensajes salientes
*/

int   
ProcessSendPack(byte ** data, size_t size)
{
    return WriteIPC(*data, size)>0?OK:ERROR; //liberar data
}

int
SendStartTransfer(process_t process)
{
    session_t session;
    size_t size;
    byte * data;
    /* Codigo de la operacion */
    session.opCode = SR_DIR_REQ_OK;
    /* Direccion en donde se va a transmitir */
    session.pid    = process.pid;
    /* Directorio a transmitir */
    session.dataSize = strlen(process.dir) + 1;
    session.data = malloc(strlen(process.dir)+1);
    strcpy(session.data, process.dir);
    
    size = MakeSessionData(session, &data);
    /* Mando el aviso */
    return ProcessSendPack(&data, size);

}

int
SendStartFileTransfer(process_t process)
{
    session_t session;
    size_t size;
    byte * data;
    /* Codigo de la operacion */
    session.opCode = SR_FIL_TRAN_REQ;
    /* Direccion en donde se va a transmitir */
    session.pid    = process.pid;
    /* Directorio a transmitir */
    session.dataSize = strlen(process.dir) + 1;
    session.data = malloc(strlen(process.dir)+1);
    strcpy(session.data, process.dir);
    
    printf("El nombre del directorio perteneciente es %s\n",process.dir);
	fflush(stdout);
    
    size = MakeSessionData(session, &data);
    /* Mando el aviso */
    return ProcessSendPack(&data, size);

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
SendFile(process_t process,fileT file)
{
    printf("Entre a la funcion\n");
    fflush(stdout);

   	session_t pack;
	byte *data;
	size_t size;

	pack.opCode = SR_FIL_TRAN;
    pack.pid = getpid();
    
    printf(" 1 Le voy a mandar el archivo --%s/%s-- que pesa %d\n",file.path,file.fName,GetSize(file));
    fflush(stdout);
    
    data = ReqFile(file);
    
    printf("LEI TODO JOYA\n");
    fflush(stdout);

	/* Armo el paquete con la informacion del file a mandar */
	pack.dataSize = MakeFilePack(file, data, &pack.data );

	size = MakeSessionData(pack, &data);
	
	printf("2 Le voy a mandar el archivo --%s/%s-- que pesa %d\n",file.path,file.fName,GetSize(file));
	
	return WriteIPC(data, size)>0?OK:ERROR; 
}

int
SendDirPack(process_t process)
{
    session_t session;
    size_t size;
    byte * data;
    
    //char * userName = ConvertPIDToUserName(process.pid);

    //if(userName != NULL){
      //strcpy(session.msg, userName);
      //free(userName);
    //}
		
    session.dataSize = strlen(process.dir) + 1;
    session.data = malloc(session.dataSize);
    strcpy(session.data, process.dir);	
    CallTransferDir(&session);

    size = MakeSessionData(session, &data);

    return ProcessSendPack(&data, size);
}

/*int
SendFileSignal(process_t process, byte * prevData )
{
    size_t size;
    byte * data;
    session_t auxSession;
    session_t session;
    
    session.pid = process.pid;
    session.dataSize = strlen(process.dir) + 1;
    session.data = malloc(session.dataSize);
    strcpy(session.data, process.dir);  
    session.opCode = SR_FIL_TRANS;
    
    
}*/

void
GoodBye(void)
{

}

void ShutDown(void)
{
	/* AVISO A CLIENTES */

	/* BORRO CANALES DE COMUNICACION */
	RemoveDir(COMM_DIR);	

    /* DESLOGUEO USUARIOS */	
    CloseApplication();

	/* MATO PROCESOS */	
	kill(getppid(),SIGINT);
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
    char *  aux;
    process_t p;
    byte * d;
    size_t size;
	switch( (*data).opCode ) {
			
		case CL_NEW_CON:
			printf("Llego un pedido de nueva conexion\n");
			fflush(stdout);
			p.status = CallNewConection(data);
			p.opCode = __NOT_SPAWN__;
			/* contesto a esta direccion */
			p.pid = (*data).pid;
			break;
			
		case CL_NEW_USR:
			printf("Llego un pedido de nueva usuario\n");
			fflush(stdout);
			p.pid = (*data).pid;
			p.status = CallNewClient(data);
			p.opCode = __NOT_SPAWN__;
			break;

		case CL_DIR_REM:
		    printf("Llego un pedido de remover directorio\n");
		    fflush(stdout);	 
		    p.status = CallDirRem(*data);
		    if(p.status==__SHUT_DOWN__)
		      fopen("apagar","w+");
		    else
		      fopen("noapagar","w+");
		    p.opCode = __NO_RESPONSE__;
		break;
			
		case CL_FIL_ADD_TRANSFER:	
		    sscanf((*data).senderID, "%d", &(p.status ));
		    p.pid = (*data).pid; 
		    (*data).opCode = SR_READY_TO_RECIEVE_ADD;
		    fopen("llegoadd","w+");
		    p.opCode = __SPAWN_REC_DEMAND__;
		    break;
		case CL_FIL_MOD_TRANSFER:
		    sscanf((*data).senderID, "%d", &(p.status ));
		    (*data).opCode = SR_READY_TO_RECIEVE_MOD;
		    aux = ExtractDirFromPath(((fileT*)((*data).data))->path);
		    strcpy(p.dir, aux);
		    p.pid = (*data).pid; 
		    p.opCode = __NOT_SPAWN__;
		    break;

		case CL_FIL_TRAN:
		    // Recibo un archivo
		    printf("Recibi el archivo!!!\n");
			fflush(stdout);	
			p.status = CallFileTransfer(*data);
			p.opCode = __NO_RESPONSE__;
			p.status = OK;
			p.pid = (*data).pid;
			strcpy(p.dir,(*data).senderID);
			break;
			
		case CL_FIL_MOD:
		    fopen("llegomodfile","w+");
		    p.status = CallFileMod(*data);
		    fopen("llegomodfile2","w+");
		    //  sscanf((*data).senderID, "%d", &(p.status));
		    p.pid = (*data).pid;
		    (*data).opCode = SR_FIL_MOD;
		 
		    aux = DirName(((fileT*)((*data).data))->path);
		    strcpy(p.dir, aux);
		    p.opCode = __NO_RESPONSE__;


		    size = MakeSessionData(*data, &d);

		    DirBroadcastMsg(p, size, d);
		    break;
		case CL_FIL_MOD_SIGNAL:
			printf("Llego un pedido de modificar directorio\n");
		    fflush(stdout);
		    /* Aca voy a escuchar */	 
		    p.pid = (*data).pid;
	        p.status = OK;
	        p.opCode = __SPAWN_REC_DEMAND__;
	        strcpy(p.dir,DirName((*data).senderID) );
	        p.aux_pid = atoi(data->msg);
	        
	        
	        /* Registro la accion */
	        CallFileMod((*data));
	        
		    //aux = DirName(((fileT*)((*data).data))->path);
		    //strcpy(p.dir, aux);

		    size = MakeSessionData(*data, &d);

		    DirBroadcastMsg(p, size, d);
		    break;
		case CL_FIL_REM:
	            p.pid = (*data).pid;
       		    aux = DirName(((fileT*)((*data).data))->path);
        	    strcpy(p.dir, aux);
        	    p.status = CallFileRem(data);
        	    p.opCode = __DIR_BROADCAST__;
		    break;
				
		case CL_DIR_LST:
			printf("Llego un pedido de listar directorios\n");
			fflush(stdout);
			p.pid = (*data).pid;	
			p.status = CallDirList(data);
			p.opCode = __NOT_SPAWN__;
			break;
			
		case CL_DIR_REQ:
			printf("Llego un pedido de agregar directorio\n");
			fflush(stdout);
			p.status = CallDirReq(data);
			fflush(stdout);
			/* Creo un proceso para pasar los archivos */
			p.opCode = __SPAWN_DEMAND__;
			/* Copio el path del directorio a sincronizar */
			strcpy(p.dir, (*data).data);
		    /* Copio la direccion del cliente al cual avisar sobre el comienzo de descarga */
			p.pid = (*data).pid;
			break;
			
		case CL_DIR_CON:
		    (*data).opCode = SR_DIR_CON_OK;
		    p.opCode = __SPAWN_DEMAND__;  
		    p.pid = (*data).pid; 
		    sscanf((*data).senderID, "%d", &(p.status));        
		    strcpy(p.dir, (*data).data);
			break;	
			
		case CL_EXT:
			printf("Llego un pedido de desloguear usuario\n");
			fflush(stdout);	
			p.status = CallClientExit(*data);
			p.opCode = __NO_RESPONSE__;
			break;	
		
		case PR_EXT:
			p.status = __SHUT_DOWN__;
			p.opCode = __NOT_SPAWN__;
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

