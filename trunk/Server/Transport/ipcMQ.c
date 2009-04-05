#include "ipcMQ.h"
#include "ipcInterface.h"

static int queue_id=0;
int IPCStarted = FALSE;
int isChildProcess = FALSE;
static int recibidos=0;

#define MAX_SIZE 512
#define SERVER_MTYPE 0
#define CLIENT_MTYPE 1

struct mymsg{
    long mtype;
    char mtext[MAX_SIZE];
};

int
InitIPC(key_t key)
{
    /*Se crea la cola de mensajes con permisos __DEFAULT_FIFO_MODE__.*/
    queue_id=msgget(key,IPC_CREAT | __DEFAULT_FIFO_MODE__);
    
    if( queue_id<0 )
    {
	printf("Error.\n");
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
    struct mymsg w_data;
    /*El size del dato a almacenar no puede ser mayor del tamaño que se puede
      almacenar en la estructura.*/
    if(size>MAX_SIZE)
	return ERROR;
    
    w_data.mtype=(long)SERVER_MTYPE;
    memcpy(w_data.mtext,data,size);
    /*El envio del mensaje espera a que se liberen recursos para poder efectuar la
      operacion en caso de que los recursos no esten disponibles.*/
    msgsnd(queue_id,&w_data,size,0);
    

    return OK;
}

byte**
ReadIPC(void)
{
    struct mymsg w_data;
    int status=OK;
    headerIPC_t header;
    byte *data;

    status=msgrcv(queue_id,&w_data,sizeof(headerIPC_t),(long)CLIENT_MTYPE,IPC_NOWAIT);
    memcpy(&header,w_data.mtext,sizeof(headerIPC_t));
    if(status > 0)
    {
	printf("\n\npaquete numero: %d\n", header.nPacket);

	if( (data = (byte *)malloc(header.size * sizeof(byte))) == NULL)
	{
	    return NULL;
	}   

	status=msgrcv(queue_id,&w_data,header.size,(long)CLIENT_MTYPE,IPC_NOWAIT);
	if(status > 0)
	{
	    memcpy(data,w_data.mtext,header.size * sizeof(byte));
	    status = OK;
	    printf("recibidos: %d\n", recibidos);
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
    return status == ERROR ? NULL: &data ;
}

void
CloseIPC(void)
{
    msgctl(queue_id,IPC_RMID,NULL);
    return ;
}

