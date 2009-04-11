#include "ipcSM.h"
#include "ipcInterface.h"

#define MAX_SIZE 1024
#define ERR_HEADER ((headerIPC_t * )-1)
#define ERR_DATA ((void * )-1)

int IPCStarted = FALSE;
int isChildProcess = FALSE;
static int recibidos;
static int shmid=-1;
static int semid=-1;
headerIPC_t * headerAux;
void * dataAux;

typedef union _semun{
    int val;
    struct semid_ds * buf;
    ushort * array;
} semun;

struct sembuf p1={0,-1,0}, p2={1,-1,0};
struct sembuf v1={0,1,0}, v2={1,1,0};

static int
GetSem(key_t key)
{
    semun x;
    x.val=0;
    if( (semid=semget(key,2,IPC_CREAT | __DEFAULT_FIFO_MODE__))==-1 )
    {
	printf("Aca 1\n");
	return ERROR;
    }
    if(semctl(semid,0,SETVAL,x)==-1)
    {
	printf("Aca 2\n");
	return ERROR;
    }
        x.val=1;
    if(semctl(semid,1,SETVAL,x)==-1)
    {
	printf("Aca 3\n");
	return ERROR;
    }
    
    return semid;
}


int
InitIPC(key_t key)
{
    int keyAux;
    /*Se crea la cola de mensajes con permisos __DEFAULT_FIFO_MODE__.*/
    if(key==0)
	keyAux=ftok("/",key);
    shmid=shmget(keyAux,MAX_SIZE,IPC_CREAT | __DEFAULT_FIFO_MODE__);
    if( shmid==-1 )
    {
	printf("Hola 1\n");
	return ERROR;
    }
    dataAux=shmat(shmid,NULL,0);
    if(dataAux==ERR_HEADER )
    {
	printf("Hola 2\n");
	return ERROR;
    }
    keyAux=ftok("/root",keyAux);
    if( GetSem(keyAux)==-1 )
    {
	printf("Hola 3\n");
	return ERROR;
    }

    /* IPCStarted = isChildProcess = TRUE pues es para un proceso hijo
    */
    IPCStarted = TRUE;
    isChildProcess = TRUE;
        printf("shmid=(%d) - semid=(%d)\n",shmid,semid);
    return OK;
}

int 
WriteIPC(void * data, size_t size)
{
    headerIPC_t header;
    header.size=size;
    header.nPacket=1;
    
    memcpy(dataAux,&header,sizeof(headerIPC_t));
    semop(semid,&v1,1);
    semop(semid,&p2,1);
    
    memcpy(dataAux,data,size);
    semop(semid,&v1,1);
    semop(semid,&p2,1);
    return OK;
}

byte**
ReadIPC(void)
{
    int status=OK;
    headerIPC_t header;
    byte *data;
    semop(semid,&p1,1);
    semop(semid,&v2,1);
    if( memcpy(&header,dataAux,sizeof(headerIPC_t)) != &header )
	return NULL;


    if(status > 0)
    {
	printf("\n\npaquete numero: %d\n", header.nPacket);
	if( (data = (byte *)malloc(header.size * sizeof(byte))) == NULL)
	{
	    return NULL;
	}   
	semop(semid,&p1,1);
	semop(semid,&v2,1);
	memcpy(data,dataAux,header.size * sizeof(byte));

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
    return status == ERROR ? NULL: &data ;
}

void
CloseIPC(void)
{
    return ;
}

