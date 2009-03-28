#include "ipcFifo.h"
#include "ipcInterface.h"
#include "TransportDefines.h"

/*por ahora, solamente comunica 2 procesos */
char * readFifo_PathArray = NULL;
char * writeFifo_PathArray = NULL;
int recibidos = 0;
int mainFifo_FD,
    writeFifo_FD,
    readFifo_FD;

int IPCStarted = FALSE;
int isChildProcess = FALSE;


/* Funcion: InitIPC()
*  ------------------
*  Crea un ipc para comunicarse con el proceso con id = ClientPid
*  El parametro pid esta en realidad para unificar los ipcs
*/
int InitIPC(key_t key)
{
    int status = OK;
    if( readFifo_PathArray == NULL && readFifo_PathArray == NULL)
    {
        status = ((readFifo_PathArray = calloc(MAX_PATH, sizeof(char))) != NULL &&
                 (writeFifo_PathArray = calloc(MAX_PATH, sizeof(char))) != NULL);
    }
    if(status != OK)
    {
        return ERROR;
    }

    /* Se arman los nombres de los fifos para el proceso que se pidio, 
    *  y luego se crean los fifo's propiamente.
    */
    sprintf(readFifo_PathArray,"%d_rd",(int)key);
    sprintf(writeFifo_PathArray,"%d_wr",(int)key);

    if ( mkfifo(readFifo_PathArray, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST)
            return ERROR;
    }
	
    if ( mkfifo(writeFifo_PathArray, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST)
    	   return ERROR;
    }

    writeFifo_FD = open(writeFifo_PathArray, O_RDWR );
    readFifo_FD = open(readFifo_PathArray, O_RDWR  );

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
    headerIPC_t header;
	status = read(readFifo_FD, &header, sizeof(headerIPC_t));
    if(status>0)
    {
		printf("\n\npaquete numero: %d\n", header.nPacket);
        status=read(readFifo_FD, data, header.size);
		if(status > 0)
		{
			printf("recibidos: %d\n", recibidos);
			recibidos++;
		}
    }

    return (status <= 0) ? ERROR :OK;
}

void
CloseIPC(void)
{
    /*Liberar recursos...*/
}

