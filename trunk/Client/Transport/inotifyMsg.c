#include "inotifyMsg.h"


char * rdFifo = NULL;

int rdFifo_FD;


static char *
MakeRD(pid_t pid,char *dir)
{
    char * path;
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return NULL;

    sprintf(path,"%s%s%d%s", COMM_DIR, INOT,(int)pid,dir);

    return path;
}


int InitINotifyMsg(pid_t pid,char *dir)
{

    int status = OK;
    /* Se arman los nombres de los fifos para el inotify que se pidio, 
    *  y luego se crean los fifo's propiamente.
    */
    
    printf("Recibi un pedido para crear un fifo con el pid %d dir %s\n",pid,dir);
    fflush(stdout); 

    rdFifo=MakeRD(pid,dir);

    if ( mkfifo(rdFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        printf("Hubo un error al crearlo\n");
        fflush(stdout);
        if(errno != EEXIST)
	{
	    status = ERROR;
	    return status;
	}
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
    while(ReadINotifyMsg() != __INOTIFY_NO_DATA__)
        ;
    status = write(rdFifo_FD, &msg, sizeof(char));
    
    printf("Mande 1 paquete al Inotify por %s **%c**\n", rdFifo, msg);
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





