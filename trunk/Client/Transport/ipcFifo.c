#include "ipcFifo.h"
#include "ipcInterface.h"

/*
* Implementacion de FIFO analoga a la del servidor, pero que 
* escribe en el fifo de lectura y lee del de escritura, pues
* de lo contrario no habria comunicacion.
*/
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
*  Crea un ipc para comunicarse con el proceso univocamente 
*  representado por key
*/

byte ** 
ReadIPC(void)
{
    int status = OK;
    headerIPC_t header;
    byte * data;
    status = read(readFifo_FD, &header, sizeof(headerIPC_t));
    if(status > 0)
    {
	    printf("\n\npaquete numero: %d\n", header.nPacket);

	    if( (data = (byte *)malloc(header.size * sizeof(byte))) == NULL)
	    {
		return NULL;
	    }   

	    status=read(readFifo_FD, data, header.size);
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

    return status == ERROR ? NULL: &data ;
}

int 
WriteIPC(void * data, size_t size)
{
    /******************* ver si es necesario poner locks *************/
    int status=write(readFifo_FD,data,size);

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
    status = read(writeFifo_FD, &header, sizeof(headerIPC_t));
    if(status>0)
    {
	printf("\n\npaquete numero: %d\n", header.nPacket);
	pid=header.pid;
        status=read(writeFifo_FD, data, header.size);
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

