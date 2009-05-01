#include "inotifyMsg.h"


char * rdFifo = NULL;

int rdFifo_FD;


static char *
MakeRD(pid_t pid)
{
    char * path;
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return NULL;

    sprintf(path,"%s%s%d", COMM_DIR, INOT,(int)pid);

    return path;
}


int InitINotifyMsg(pid_t pid)
{

    int status = OK;
    /* Se arman los nombres de los fifos para el inotify que se pidio, 
    *  y luego se crean los fifo's propiamente.
    */

    rdFifo=MakeRD(pid);

    if ( mkfifo(rdFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST)
          return ERROR;
    }

    rdFifo_FD = open(rdFifo, O_RDWR | O_NONBLOCK);

    if(rdFifo_FD == ERROR )
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

    status = write(rdFifo_FD, &msg, sizeof(char));

    return status == ERROR ? ERROR: OK ;
}

char 
ReadINotifyMsg(void)
{
    int status = OK;
    char msg = __INOTIFY_NO_DATA__;

    status = read(rdFifo_FD, &msg, sizeof(char));
   
    return (status == ERROR || status == 0) ? __INOTIFY_NO_DATA__ : msg ;
}

void
CloseINotifyMsg(void)
{
    /*Liberar recursos...*/
}




