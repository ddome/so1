#include "ipcMQ.h"
#include "ipcInterface.h"

static int queue_id=0;
int IPCStarted = FALSE;
int isChildProcess = FALSE;
static int recibidos=0;

#define MAX_SIZE 1024
#define SERVER_MTYPE 1
#define CLIENT_MTYPE 3

struct mymsg{
    long mtype;
    char mtext[MAX_SIZE];
};

int
InitIPC(key_t key)
{
    /*Se crea la cola de mensajes con permisos __DEFAULT_FIFO_MODE__.*/
    queue_id=msgget(11223444,IPC_CREAT | __DEFAULT_FIFO_MODE__);
    printf("key: %d\n",key);
    if( queue_id<0 )
    {
	return ERROR;
    }
    printf("queue id:%d\n",queue_id);
    /* IPCStarted = isChildProcess = TRUE pues es para un proceso hijo
    */
    IPCStarted = TRUE;
    isChildProcess = TRUE;
    return OK;
}

int 
WriteIPC(void * data, size_t size)
{
    int status;
    struct mymsg w_data;
    headerIPC_t header;
    header.nPacket = 1;
    header.size = size;
    /*El size del dato a almacenar no puede ser mayor del tamaño que se puede
      almacenar en la estructura.*/
    if(size>MAX_SIZE)
	return ERROR;
    w_data.mtype=(long)SERVER_MTYPE;
    memcpy(&(w_data.mtext),&header,sizeof(headerIPC_t));
    status=msgsnd(queue_id,&w_data,sizeof(headerIPC_t),IPC_NOWAIT);
    w_data.mtype=(long)SERVER_MTYPE;
    memcpy(&(w_data.mtext),data,size);
    /*El envio del mensaje espera a que se liberen recursos para poder efectuar la
      operacion en caso de que los recursos no esten disponibles.*/
    status=msgsnd(queue_id,&w_data,size,IPC_NOWAIT);
    return (status<0)?ERROR:OK;
}

byte*
ReadIPC(void)
{
    struct mymsg w_data;
    int status=OK;
    headerIPC_t header;
    byte *data;
    
    status=msgrcv(queue_id,&w_data,MAX_SIZE,(long)CLIENT_MTYPE,IPC_NOWAIT);
    if(status > 0)
    {
	memcpy(&header,w_data.mtext,sizeof(headerIPC_t));
	printf("\n\npaquete numero: %d\n", header.nPacket);

	if( (data = (byte *)malloc(header.size * sizeof(byte))) == NULL)
	{
	    return NULL;
	}   
	status=msgrcv(queue_id,&w_data,MAX_SIZE,(long)CLIENT_MTYPE,IPC_NOWAIT);
	if(status > 0)
	{
	    memcpy(data,w_data.mtext,header.size * sizeof(byte));
	    status = OK;
	    printf("recibidos: %d\n", recibidos);
	    recibidos++;
	}
	else
	{
	    if(errno==E2BIG)
		printf("The  message text length is greater than msgsz and MSG_NOER-ROR isn’t specified in msgflg.\n");
	    else if(errno==EACCES)
		printf("The calling process does not have  read  permission  on  the message  queue, and does not have the CAP_IPC_OWNER capabil‐ity.\n");
	    else if(errno==EAGAIN)
		printf("No message was available in the  queue  and  IPC_NOWAIT  was specified in msgflg.\n");
	    else if(errno==EFAULT)
		printf("The address pointed to by msgp isn’t accessible.\n");
	    else if(errno==EIDRM)
		printf("While  the  process  was  sleeping to receive a message, the message queue was removed.\n");
	    else if(errno==EINTR)
		printf("While the process was sleeping to  receive  a  message,  the process caught a signal.\n");
	    else if(errno==EINVAL)
		printf("msgqid was invalid, or msgsz was less than 0.\n");
	    else if(errno==ENOMSG)
		/*printf("IPC_NOWAIT  was  specified  in  msgflg and no message of the requested type existed on the message queue.\n")*/;
	    else
		printf("Otro error.\n");
	    status = ERROR;
	}
    }
    else
    {
	if(errno==E2BIG)
	    printf("The  message text length is greater than msgsz and MSG_NOER-ROR isn’t specified in msgflg.\n");
	else if(errno==EACCES)
	    printf("The calling process does not have  read  permission  on  the message  queue, and does not have the CAP_IPC_OWNER capabil‐ity.\n");
	else if(errno==EAGAIN)
	    printf("No message was available in the  queue  and  IPC_NOWAIT  was specified in msgflg.\n");
	else if(errno==EFAULT)
	    printf("The address pointed to by msgp isn’t accessible.\n");
	else if(errno==EIDRM)
	    printf("While  the  process  was  sleeping to receive a message, the message queue was removed.\n");
	else if(errno==EINTR)
	    printf("While the process was sleeping to  receive  a  message,  the process caught a signal.\n");
	else if(errno==EINVAL)
	    printf("msgqid was invalid, or msgsz was less than 0.\n");
	else if(errno==ENOMSG)
	    /*printf("IPC_NOWAIT  was  specified  in  msgflg and no message of the requested type existed on the message queue.\n")*/;
	else
	    printf("Otro error.\n");
	status = ERROR;
    }

    return status == ERROR ? NULL: data ;
}

void
CloseIPC(void)
{
    msgctl(queue_id,IPC_RMID,NULL);
    return ;
}

