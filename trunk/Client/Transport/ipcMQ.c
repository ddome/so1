#include "ipcMQ.h"
#include "ipcInterface.h"

static int queue_id=0;
int IPCStarted = FALSE;
int isChildProcess = FALSE;
static int recibidos=0;

#define MAX_SIZE 2056
#define SERVER_MTYPE 1
#define CLIENT_MTYPE 3

struct mymsg{
    long mtype;
    char mtext[MAX_SIZE];
};

static byte *
GetBlock(byte *org, size_t size, int index)
{
	byte *block=NULL;
	
	if( (block=calloc(size,sizeof(byte))) == NULL ) {
		return NULL;
	}
	
	return memmove(block, org+index*size, size);
}

static int
GetTotalPackets(size_t size)
{
	return (int)(size / PACKET_SIZE + 1) ;
}

int
InitIPC(key_t key)
{
    /*Se crea la cola de mensajes con permisos __DEFAULT_FIFO_MODE__.*/
    if(key==0)
	key=ftok("/",key);
    queue_id=msgget(key,__DEFAULT_FIFO_MODE__);
    if( queue_id<0 )
    {
	    return ERROR;
    }
    /* IPCStarted = isChildProcess = TRUE pues es para un proceso hijo
    */
    IPCStarted = TRUE;
    isChildProcess = TRUE;
    return OK;
}

int 
WriteIPC(void * data, size_t size)
{
    int status,i,bytesLeft,npacket;
    struct mymsg w_data;
    byte *block;
    headerIPC_t header;
    
    bytesLeft = size;
    npacket=1;

    header.totalPackets = GetTotalPackets(size);
    
    for( i=0; i < header.totalPackets; i++ )
    {
	    header.nPacket = npacket;
	    header.size = PACKET_SIZE;
	    w_data.mtype=(long)CLIENT_MTYPE;
	    memcpy(w_data.mtext,&header,sizeof(headerIPC_t));
	    status=msgsnd(queue_id,&w_data,sizeof(headerIPC_t),0);
	
	    if(status != ERROR)
	    {
		    block = GetBlock(data, header.size, npacket-1);
		    w_data.mtype=(long)CLIENT_MTYPE;
		    memcpy(w_data.mtext,block,header.size);
		    status=msgsnd(queue_id,&w_data,header.size,0);
		    free(block);
		    if( status == ERROR )
			return ERROR;
	    }
	    bytesLeft -= PACKET_SIZE;
	    npacket++;
    }
    printf("Enviados: %d\n",npacket-1);

    return (status<0)?ERROR:OK;
}

byte*
ReadIPC(void)
{
    int status = OK,pos,nPacketsRead;
    struct mymsg w_data;
    headerIPC_t header;
    byte * data=NULL;
    byte *aux;
    int prueba=0;
    nPacketsRead=0;
    pos=0;
    printf("Descargando paquetes...");
    do{
	status=msgrcv(queue_id,&w_data,sizeof(headerIPC_t),(long)SERVER_MTYPE,0);
	
	if(status > 0)
	{
	    memcpy(&header,w_data.mtext,sizeof(headerIPC_t));
	    printf("totalPackets= %d\n",header.totalPackets );
	    data = realloc(data, pos + header.size );
	    aux = malloc(header.size);
	    
	    status=msgrcv(queue_id,&w_data,header.size,(long)SERVER_MTYPE,0);
	    memcpy(aux,w_data.mtext,header.size);
	    
	    memmove(data+pos, aux, header.size);
	    pos += header.size;
	    nPacketsRead++;
	    prueba=1;
	    if(status > 0)
	    {
		status = OK;
		recibidos++;
	    }
	    else
	    {
		status = ERROR;
	    }			  
	}
	else
	{
	    status = ERROR;
	}	
    }while( status != ERROR && nPacketsRead < header.totalPackets );
    if(prueba)
	printf("Sali\n");
    if(nPacketsRead!=0)
	printf("recibidos: %d\n", nPacketsRead);
    else
	getchar();
    return status == ERROR ? NULL: data ;
}

void
CloseIPC(void)
{
    msgctl(queue_id,IPC_RMID,NULL);
    return ;
}

