#include "ipcFifo.h"
#include "ipcInterface.h"

/*por ahora, solamente comunica 2 procesos */
char * readFifo = NULL;
char * writeFifo = NULL;
int recibidos = 0;
int mainFifo_FD,
    writeFifo_FD,
    readFifo_FD;

int IPCStarted = FALSE;
int isChildProcess = FALSE;

string
MakeRDPath(key_t key)
{
    string path;
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return NULL;

    sprintf(path,"%s%d_rd",COMM_DIR,(int)key);

    return path;
}

string
MakeWRPath(key_t key)
{
    string path;
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return NULL;

    sprintf(path,"%s%d_wr",COMM_DIR,(int)key);

    return path;
}


/* Funcion: InitIPC()
*  ------------------
*  Crea un ipc para comunicarse con el proceso con id = ClientPid
*  El parametro pid esta en realidad para unificar los ipcs
*/

int InitIPC(key_t key)
{
    /* Se arman los nombres de los fifos para el proceso que se pidio, 
    *  y luego se crean los fifo's propiamente.
    */
    
    readFifo=MakeRDPath(key);
    writeFifo=MakeWRPath(key);

    if ( mkfifo(readFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST)
            return ERROR;
    }
    if ( mkfifo(writeFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST)
    	   return ERROR;
    }
    writeFifo_FD = open(writeFifo, O_RDWR );
    readFifo_FD = open(readFifo, O_RDWR  );

    if(readFifo_FD == -1 || writeFifo_FD == -1 )
    {
		printf("no se puede abrir el fifo");
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
    /******************* ver si es necesario poner locks *************/
    int status=write(writeFifo_FD,data,size);

    return OK;
}

/*habria que meter en el void * un header indicando el numero de paquete
y hacer lecturas mientras haya*/
pid_t
ReadIPC(void * data)
{
    int status = OK;
    pid_t pid=-1;
    headerIPC_t header;
    status = read(readFifo_FD, &header, sizeof(headerIPC_t));
    if(status>0)
    {
	printf("\n\npaquete numero: %d\n", header.nPacket);
	pid=header.pid;
        status=read(readFifo_FD, data, header.size);
	if(status > 0)
	{
	    printf("recibidos: %d\n", recibidos);
	    recibidos++;
	}
    }

    return (status <= 0) ? ERROR :pid;
}

void
CloseIPC(void)
{
    /*Liberar recursos...*/
}


