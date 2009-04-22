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
	
	if( (block=malloc(size*sizeof(byte))) == NULL ) {
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
    queue_id=msgget(key,IPC_CREAT | __DEFAULT_FIFO_MODE__);
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
    
    header.nPacket = 1;
    header.size = size;
    header.totalPackets = GetTotalPackets(size);
    
    for( i=0; i < header.totalPackets; i++ )
    {
	    header.nPacket = npacket;
	    header.size = PACKET_SIZE;
	    printf("totalPackets= %d\n",header.totalPackets );
	    w_data.mtype=(long)SERVER_MTYPE;
	    memcpy(w_data.mtext,&header,sizeof(headerIPC_t));
	    status=msgsnd(queue_id,&w_data,sizeof(headerIPC_t),0);
	
	    if(status != ERROR)
	    {
		    block = GetBlock(data, header.size, npacket-1);
		    w_data.mtype=(long)SERVER_MTYPE;
		    memcpy(w_data.mtext,block,header.size);
		    status=msgsnd(queue_id,&w_data,header.size,0);
		    printf("status=%d\n",status);
		    free(block);
		    if( status == ERROR )
			return ERROR;
	    }
	    else
		return ERROR;
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
    do{
	status=msgrcv(queue_id,&w_data,sizeof(headerIPC_t),(long)CLIENT_MTYPE,0);

	if(status > 0)
	{
	    memcpy(&header,w_data.mtext,sizeof(headerIPC_t));
	    printf("Aca 3\n");
	    data = realloc(data, pos + header.size );
	    aux = malloc(header.size);
	    printf("Aca 4\n");
	    
	    status=msgrcv(queue_id,&w_data,header.size,(long)CLIENT_MTYPE,0);
	    memcpy(aux,w_data.mtext,header.size);
	    printf("Aca 5\n");
	    memmove(data+pos, aux, header.size);
	    printf("Aca 6\n");
	    pos += header.size;
	    printf("Aca 7\n");
	    nPacketsRead++;
	    prueba=1;
	    printf("Aca 8\n");
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
    return status == ERROR ? NULL: data ;
}

void
CloseIPC(void)
{
    msgctl(queue_id,IPC_RMID,NULL);
    return ;
}

