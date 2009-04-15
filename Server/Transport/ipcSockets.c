#include "ipcSockets.h"
#include "ipcInterface.h"

static int socketFD=0;
struct sockaddr_in server = {AF_INET,7008};
struct sockaddr_in client = {AF_INET,8008};
static int recibidos=0;
static int connected=0;


#define QUEUE_SIZE_MAX 5

int
InitIPC(key_t key)
{
    if( (socketFD=socket(AF_INET,SOCK_STREAM,0))==-1 )
	return ERROR;
    
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if( bind(socketFD,(struct sockaddr *)&server,sizeof(struct sockaddr))==-1 )
	return ERROR;
    
    printf("Socket FD: %d\n",socketFD);
    
    if(listen(socketFD,QUEUE_SIZE_MAX)==-1)
	return ERROR;
    return OK; 
    
}


int 
WriteIPC(void * data, size_t size)
{
    int status;
    headerIPC_t header;
    server.sin_addr.s_addr=INADDR_ANY;
    if( connect(socketFD,(struct sockaddr *)&server,sizeof(struct sockaddr))==-1 )
	return ERROR;
    header.nPacket = 1;
    header.size = size;

    status=send(socketFD,&header,sizeof(headerIPC_t),0);

    status=send(socketFD,data,size,0);
    return (status<0)?ERROR:OK;
}
/*CAMBIAR PROTOTIPO*/
byte*
ReadIPC(void)
{
    int status=OK;
    int newsockFD;
    headerIPC_t header;
    byte *data;

    /*if(!connected)
    {*/
    printf("hola 1\n");
	if( (newsockFD=accept(socketFD,NULL,NULL))==-1)
	{
	    return NULL;
	}
	/*connected=1;
    }*/
    printf("hola 2\n");
    status=recv(newsockFD,&header,sizeof(headerIPC_t),0);
    if(status > 0)
    {
	printf("\n\npaquete numero: %d\n", header.nPacket);

	if( (data = (byte *)malloc(header.size * sizeof(byte))) == NULL)
	{
	    close(newsockFD);
	    connected=0;
	    return NULL;
	}   

	status=recv(newsockFD,data,header.size,0);
	if(status > 0)
	{
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
    close(newsockFD);
    connected=0;
    return status == ERROR ? NULL: data ;
}


void
CloseIPC(void)
{
}





























































































