#include "ipcSM.h"
#include "ipcInterface.h"

#define MAX_SIZE 1024
#define ERR_DATA ((void * )-1)

int IPCStarted = FALSE;
int isChildProcess = FALSE;
static int recibidos;
static int shmid1=-1;
static int shmid2=-1;
static int semid=-1;
static void * dataAux1;
static void * dataAux2;


typedef union _semun{
    int val;
    struct semid_ds * buf;
    ushort * array;
} semun;

struct sembuf p1={0,-1,SEM_UNDO}, p2={1,-1,SEM_UNDO},p3={2,-1,SEM_UNDO}, p4={3,-1,SEM_UNDO};
struct sembuf v1={0,1,SEM_UNDO}, v2={1,1,SEM_UNDO},v3={2,1,SEM_UNDO}, v4={3,1,SEM_UNDO};

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

static int
GetSem(key_t key)
{
    semun x;
    x.val=0;
    if( (semid=semget(key,4, __DEFAULT_FIFO_MODE__))==-1 )
    {
	printf("Aca 1\n");
	return ERROR;
    }
    /*if(semctl(semid,0,SETVAL,x)==-1)
    {
	printf("Aca 2\n");
	return ERROR;
    }
    if(semctl(semid,1,SETVAL,x)==-1)
    {
	printf("Aca 3\n");
	return ERROR;
    }
    if(semctl(semid,2,SETVAL,x)==-1)
    {
	printf("Aca 2\n");
	return ERROR;
    }
    if(semctl(semid,3,SETVAL,x)==-1)
    {
	printf("Aca 3\n");
	return ERROR;
    }*/
    
    return semid;
}


int
InitIPC(key_t key)
{
    int keyAux1,keyAux2;
    /*Se crea la cola de mensajes con permisos __DEFAULT_FIFO_MODE__.*/
    keyAux1=ftok("/tmp",key);
    keyAux2=ftok("/home",key);
    shmid1=shmget(keyAux1,MAX_SIZE, __DEFAULT_FIFO_MODE__);
    shmid2=shmget(keyAux2,MAX_SIZE, __DEFAULT_FIFO_MODE__);
    if( shmid1==-1 || shmid2==-1 )
    {
	return ERROR;
    }
    dataAux1=shmat(shmid1,NULL,0);
    dataAux2=shmat(shmid2,NULL,0);
    if(dataAux1==ERR_DATA || dataAux2==ERR_DATA)
    {
	printf("Hola 2\n");
	return ERROR;
    }
    keyAux1=ftok("/home",keyAux1);
    if( GetSem(keyAux1)==-1 )
    {
	printf("Hola 3\n");
	return ERROR;
    }

    /* IPCStarted = isChildProcess = TRUE pues es para un proceso hijo
    */
    IPCStarted = TRUE;
    isChildProcess = TRUE;
    /*printf("shmid1=(%d) - shmid2=(%d) - semid=(%d)\n",shmid1,shmid2,semid);*/
    return OK;
}

int 
WriteIPC(void * data, size_t size)
{
    int status,i,bytesLeft,npacket;
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
	    printf("Write 1\n");
	    memcpy(dataAux1,&header,sizeof(headerIPC_t));
	    semop(semid,&v1,1);
	    semop(semid,&p2,1);
	    printf("Write 2\n");
	    if(status != ERROR)
	    {
		    block = GetBlock(data, header.size, npacket-1);
		    printf("Write 3\n");
		    memcpy(dataAux1,block,header.size);
		    semop(semid,&v1,1);
		    semop(semid,&p2,1);
		    printf("Write 4\n");
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
    /*headerIPC_t header;
    header.size=size;
    header.nPacket=1;
    
    memcpy(dataAux1,&header,sizeof(headerIPC_t));
    semop(semid,&v1,1);
    semop(semid,&p2,1);
    
    memcpy(dataAux1,data,size);
    semop(semid,&v1,1);
    semop(semid,&p2,1);
    return OK;*/
}

byte*
ReadIPC(void)
{
    int status = OK,pos,nPacketsRead;
    headerIPC_t header;
    byte * data=NULL;
    byte *aux;
    int prueba=0;
    nPacketsRead=0;
    pos=0;
    do{
	semop(semid,&p3,1);
	memcpy(&header,dataAux2,sizeof(headerIPC_t));
	semop(semid,&v4,1);

	if(status > 0)
	{
	    printf("Read 3\n");
	    data = realloc(data, pos + header.size );
	    aux = malloc(header.size);
	    printf("Read 4\n");
	    
	    semop(semid,&p3,1);
	    memcpy(aux,dataAux2,header.size);
	    semop(semid,&v4,1);

	    printf("Read 5\n");
	    memmove(data+pos, aux, header.size);
	    printf("Read 6\n");
	    pos += header.size;
	    printf("Read 7\n");
	    nPacketsRead++;
	    prueba=1;
	    printf("Read 8\n");
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
/*
    int status=OK;
    headerIPC_t header;
    byte *data;
    semop(semid,&p3,1);
    
    if( memcpy(&header,dataAux2,sizeof(headerIPC_t)) != &header )
	return NULL;
    semop(semid,&v4,1);

    if(status > 0)
    {
	printf("\n\npaquete numero: %d\n", header.nPacket);
	if( (data = (byte *)malloc(header.size * sizeof(byte))) == NULL)
	{
	    return NULL;
	}   
	semop(semid,&p3,1);
	
	memcpy(data,dataAux2,header.size * sizeof(byte));
	semop(semid,&v4,1);
	printf("Lei.\n");
	if(status > 0)
	{
	    status = OK;
	    printf("recibidos: %d\n", recibidos);
	    recibidos++;
	}
	else
	{
	    return NULL;
	}
    }
    else
    {
	status = ERROR;
    }
    return status == ERROR ? NULL: data ;*/
}

void
CloseIPC(void)
{
    return ;
}