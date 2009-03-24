#include "ipcFifo.h"
#include "ipcInterface.h"
#include "TransportDefines.h"

char * mainFifo_Path = "mainFifo";
/* Temporario: vector de paths donde a cada indice le corresponde
*  el pipe para lecura o escritura para un determinado subproceso

char ** readFifo_PathArray = NULL ;
char ** writeFifo_PathArray = NULL;
*/

/*por ahora, solamente comunica 2 procesos */
char * readFifo_PathArray = NULL;
char * writeFifo_PathArray = NULL;

int mainFifo_FD,
    writeFifo_FD,
    readFifo_FD;

int ClientPID = __DEFAULT_PID__;

int IPCStarted = FALSE;
int isChildProcess = FALSE;

/* Funcion: InitMainIPC()
*  ----------------------
*  Inicializa un FIFO en un path default, en el cual el servidor principal
*  recibira los pedidos que le conciernen.
*/
int
InitMainIPC(void)
{
    /* Se inicializa el fifo principal */
    if ( mkfifo(mainFifo_Path, __DEFAULT_FIFO_MODE__) == ERROR )
    {
            if(errno != EEXIST)
                return ERROR;
    }
    IPCStarted = TRUE;
    return OK;
}

/* Funcion: InitIPC()
*  ------------------
*  Crea un ipc para comunicarse con el proceso con id = ClientPid
*  El parametro pid esta en realidad para unificar los ipcs
*/
int InitIPC(int pid)
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
    /* Si el fifo principal no esta inicializado, se lo inicia ahora
    */
    if ( mkfifo(mainFifo_Path, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST)
            return ERROR;
    }
	
    /* Se abre el FIFO
    */
    mainFifo_FD = open(mainFifo_Path,  O_RDWR | O_NONBLOCK);
    /* Se arman los nombres de los fifos para el proceso que se pidio, 
    *  y luego se crean los fifo's propiamente.
    */
    sprintf(readFifo_PathArray,"%d_rd",(int)ClientPID);
    sprintf(writeFifo_PathArray,"%d_wr",(int)ClientPID);

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

    writeFifo_FD = open(writeFifo_PathArray, O_RDWR  | O_NONBLOCK);
    readFifo_FD = open(readFifo_PathArray, O_RDWR  | O_NONBLOCK);

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
    write(writeFifo_FD,data,size);
    return OK;
}

/*habria que meter en el void * un header indicando el numero de paquete
y hacer lecturas mientras haya*/
pid_t
ReadIPC(void * data)
{
    int status = OK;
    status = read(readFifo_FD, (char*)data, sizeof(char));
	printf("%s - %d - %d\n", readFifo_PathArray, readFifo_FD, status);

    return (status <= 0) ? ERROR : ClientPID;
}

void
CloseIPC(void)
{
    /*Liberar recursos...*/
}

