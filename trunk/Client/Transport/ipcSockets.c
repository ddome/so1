#include "ipcSockets.h"
#include "ipcInterface.h"

/*
* Implementacion de FIFO analoga a la del servidor, pero que 
* escribe en el fifo de lectura y lee del de escritura, pues
* de lo contrario no habria comunicacion.
*/
char * serverPath = NULL;
char * clientPath = NULL;
int recibidos = 0;
int socketFD;

int IPCStarted = FALSE;
int isChildProcess = FALSE;

static byte * GetBlock(byte *org, size_t size, int index);

static string
MakeRDPath(key_t key)
{
    string path;
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return NULL;

    sprintf(path,"%s%d_wr",COMM_DIR,(int)key);

    return path;
}

static string
MakeWRPath(key_t key)
{
    string path;
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return NULL;

    sprintf(path,"%s%d_rd",COMM_DIR,(int)key);

    return path;
}

int InitIPC(key_t key)
{
  serverPath=MakeRDPath(key);
  clientPath=MakeWRPath(key);


	
    struct sockaddr_un client;
    client.sun_family = AF_UNIX;
    strcpy(client.sun_path, clientPath);
    unlink(client.sun_path);
    if( (socketFD=socket(AF_UNIX,SOCK_DGRAM,0))==-1 )
    {
	printf("A comerlaaaaaaa!!!\n");
	return ERROR;
    }
    if (bind(socketFD, (struct sockaddr *)&client, sizeof(struct sockaddr_un)) == -1) 
    {
	printf("Murio\n");
	return ERROR;
    }
   
  IPCStarted = TRUE;
  isChildProcess = TRUE;

  return OK;
}

static int
GetTotalPackets(size_t size)
{
	return (int)(size / PACKET_SIZE + 1) ;
}

int 
WriteIPC(void * data, size_t size)
{
    int status;
    headerIPC_t header;
    byte *block;
    int bytesLeft;	
    
    header.totalPackets = GetTotalPackets(size);
    
    int npacket   = 1;
    bytesLeft = size;
    int i;

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, serverPath);

    for( i=0; i < header.totalPackets; i++ )
    {
	    header.nPacket = npacket;
	    header.size = PACKET_SIZE;
	    
	    status=sendto(socketFD,&header,sizeof(headerIPC_t),0,(struct sockaddr*)&server,sizeof(struct sockaddr_un));
	    if(status != ERROR)
	    {
		    block = GetBlock(data, header.size, npacket-1);
		    status=sendto(socketFD,block,header.size,0,(struct sockaddr*)&server,sizeof(struct sockaddr_un));
		    free(block);
		    
		    if( status == ERROR )
			    return ERROR;
	    }
	    
	    bytesLeft -= PACKET_SIZE;
	    npacket++;
    }
	
    return status;
}

byte *
ReadIPC(void)
{
	int status = OK;
	headerIPC_t header;
	byte * data=NULL;
	int nPacketsRead;
	int pos;
	byte *aux;
	
	printf("Descargando paquetes...");
	
	nPacketsRead=0;
	pos=0;
	do{
		status=recvfrom(socketFD,&header,sizeof(headerIPC_t),0,NULL,0);
		if(status > 0)
		{	
			data = realloc(data, pos + header.size );
			aux = malloc(header.size);			
			status=recvfrom(socketFD,aux,header.size,0,NULL,0);
			
			memmove(data+pos, aux, header.size);
			pos += header.size;
			nPacketsRead++;
			
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
	
	printf("recibidos: %d\n", recibidos);
	
	return status == ERROR ? NULL: data ;
}

void
CloseIPC(void)
{
    /*Liberar recursos...*/
}

static byte *
GetBlock(byte *org, size_t size, int index)
{
	byte *block;
	
	if( (block=malloc(size)) == NULL ) {
		return NULL;
	}
	
	return memcpy(block, org+index*size, size);
}

