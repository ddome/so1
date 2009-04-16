#include "inotifyMsg.h"


char * rdFifo = NULL;
char * wrFifo = NULL;

int wrFifo_FD,
    rdFifo_FD;


static char *
MakeRD(pid_t pid)
{
    char * path;
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return NULL;

    sprintf(path,"%s%s%d_rd", COMM_DIR, INOT,(int)pid);

    return path;
}

static char *
MakeWR(pid_t pid)
{
    char * path;
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return NULL;

    sprintf(path,"%s%s%d_wr", COMM_DIR, INOT,(int)pid);

    return path;
}

int InitINotifyMsg(pid_t pid)
{

    int status = OK;
    /* Se arman los nombres de los fifos para el inotify que se pidio, 
    *  y luego se crean los fifo's propiamente.
    */

    rdFifo=MakeRD(pid);
    wrFifo=MakeWR(pid);

    if ( mkfifo(rdFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST)
          return ERROR;
    }
    if ( mkfifo(wrFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST)
          return ERROR;
    }
    wrFifo_FD = open(wrFifo, O_RDWR );
    rdFifo_FD = open(rdFifo, O_RDWR  );

    if(rdFifo_FD == -1 || wrFifo_FD == -1 )
    {
      status = ERROR;
		printf("no se puede abrir el fifo");
    }
	

    return status;
}

int 
WriteINotifyMsg(char msg)
{
    int status;

    status = write(wrFifo_FD, &msg, sizeof(char));

    return status == ERROR ? ERROR: status ;
}

char 
ReadINotifyMsg(void)
{
    int status = OK;
    char msg;

    status = read(rdFifo_FD, &msg, sizeof(char));
   
    return (status == ERROR || status == 0) ? __NO_DATA__ : msg ;
}

void
CloseINotifyMsg(void)
{
    /*Liberar recursos...*/
}


