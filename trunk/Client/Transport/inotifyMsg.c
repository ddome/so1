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
    
    printf("Recibi un pedido para crear un fifo con el pid %d\n",pid);
    fflush(stdout); 

    rdFifo=MakeRD(pid);

    if ( mkfifo(rdFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        printf("Hubo un error al crearlo\n");
        fflush(stdout);
        if(errno != EEXIST)
          status = ERROR;
    }
    
    fflush(stdout);

    rdFifo_FD = open(rdFifo, O_RDWR | O_NONBLOCK);   
    
    printf("Cree el fifo con nombre %s\n",rdFifo);
    fflush(stdout);
	

    return status;
}

int 
WriteINotifyMsg(char msg)
{
    int status;

    status = write(rdFifo_FD, &msg, sizeof(char));
    
    printf("Mande 1 paquete al Inotify por %s \n", rdFifo);
    fflush(stdout);

    return status == ERROR ? ERROR: OK ;
}

char 
ReadINotifyMsg(void)
{
    int status = OK;
    char msg = __INOTIFY_NO_DATA__;

    status = read(rdFifo_FD, &msg, sizeof(char));
    
    //printf("Lei un Inotify por %s \n",rdFifo);
    //fflush(stdout);
   
    return (status == ERROR || status == 0) ? __INOTIFY_NO_DATA__ : msg ;
}

void
CloseINotifyMsg(void)
{
    /*Liberar recursos...*/
}




