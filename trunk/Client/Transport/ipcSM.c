#include "ipcSM.h"
#include "ipcInterface.h"

#define MAX_SIZE 1024
#define ERR_HEADER ((headerIPC_t * )-1)
#define ERR_DATA ((void * )-1)

int IPCStarted = FALSE;
int isChildProcess = FALSE;
static int recibidos;
static int shmid1=-1;
static int shmid2=-1;
static int semid=-1;
void * dataAux1;
void * dataAux2;


typedef union _semun{
    int val;
    struct semid_ds * buf;
    ushort * array;
} semun;

struct sembuf p1={0,-1,0}, p2={1,-1,0},p3={2,-1,0}, p4={3,-1,0};
struct sembuf v1={0,1,0}, v2={1,1,0},v3={2,1,0}, v4={3,1,0};

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
	printf("Hola 1\n");
	return ERROR;
    }
    dataAux1=shmat(shmid1,NULL,0);
    dataAux2=shmat(shmid2,NULL,0);
    if(dataAux1==ERR_HEADER || dataAux2==ERR_HEADER)
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
    headerIPC_t header;
    header.size=size;
    header.nPacket=1;
    
    memcpy(dataAux1,&header,sizeof(headerIPC_t));
    semop(semid,&v1,1);
    semop(semid,&p2,1);
    
    memcpy(dataAux1,data,size);
    semop(semid,&v1,1);
    semop(semid,&p2,1);
    return OK;
}

byte*
ReadIPC(void)
{
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
    return status == ERROR ? NULL: data ;
}

void
CloseIPC(void)
{
    return ;
}