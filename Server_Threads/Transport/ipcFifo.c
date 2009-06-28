#include "ipcFifo.h"
#include "ipcInterface.h"

char * readFifo = NULL;
char * writeFifo = NULL;
int recibidos = 0;
int mainFifo_FD,
    writeFifo_FD,
    readFifo_FD;
    pthread_mutex_t rdmtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t wrmtx=PTHREAD_MUTEX_INITIALIZER;
    int IPCStarted = FALSE;
    int isChildProcess = FALSE;

    static byte * GetBlock(byte *org, size_t size, int index);

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

    int status = OK;
    /* Se arman los nombres de los fifos para el proceso que se pidio, 
    *  y luego se crean los fifo's propiamente.
    */

    readFifo=MakeRDPath(key);
    writeFifo=MakeWRPath(key);

    if ( mkfifo(readFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST){
            return __ERROR_IPC_CREAT__;
        }
    }
    if ( mkfifo(writeFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST){
            return __ERROR_IPC_CREAT__;
        }
    }
    writeFifo_FD = open(writeFifo, O_RDWR );
    readFifo_FD = open(readFifo, O_RDWR);

    if(readFifo_FD == -1 || writeFifo_FD == -1 )
    {
        status = __ERROR_IPC_CREAT__;
        printf("no se puede abrir el fifo");
    }
    
    /* IPCStarted = isChildProcess = TRUE pues es para un proceso hijo
    */
    IPCStarted = TRUE;
    isChildProcess = TRUE;
    return status;
}

int InitWriteIPC(key_t key)
{
    
    int status = OK;
    /* Se arman los nombres de los fifos para el proceso que se pidio, 
    *  y luego se crean los fifo's propiamente.
    */
    
    writeFifo=MakeWRPath(key);
    

    if ( mkfifo(writeFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST){
            return __ERROR_IPC_CREAT__;
        }
    }
    writeFifo_FD = open(writeFifo, O_RDWR );
    
    if( writeFifo_FD == -1 )
    {
        status = __ERROR_IPC_CREAT__;
        printf("no se puede abrir el fifo");
    }
    
    /* IPCStarted = isChildProcess = TRUE pues es para un proceso hijo
    */
    IPCStarted = TRUE;
    isChildProcess = TRUE;
    return status;
}

int InitReadIPC(key_t key)
{
    
    int status = OK;
    /* Se arman los nombres de los fifos para el proceso que se pidio, 
    *  y luego se crean los fifo's propiamente.
    */
    
    readFifo=MakeRDPath(key);

    if ( mkfifo(readFifo, __DEFAULT_FIFO_MODE__) == ERROR )
    {
        if(errno != EEXIST){
            return __ERROR_IPC_CREAT__;
        }
    }

    readFifo_FD = open(readFifo, O_RDWR  );
    
    if(readFifo_FD == -1 )
    {
        status = __ERROR_IPC_CREAT__;
        printf("no se puede abrir el fifo");
    }

    /* IPCStarted = isChildProcess = TRUE pues es para un proceso hijo
    */
    IPCStarted = TRUE;
    isChildProcess = TRUE;
    return status;
}
static int
GetTotalPackets(size_t size)
{
    return (int)(size / PACKET_SIZE + 1) ;
}

int 
WriteIPC(key_t key, void * data, size_t size)
{
    pthread_mutex_lock(&wrmtx);
    InitWriteIPC(key);
    int status;
    headerIPC_t header;
    byte *block;
    int bytesLeft;  
    
    header.totalPackets = GetTotalPackets(size);
    
    int npacket   = 1;
    bytesLeft = size;
    int i;
    
    for( i=0; i < header.totalPackets; i++ ) {
        header.nPacket = npacket;
        header.size = PACKET_SIZE;
        
        status = write(writeFifo_FD, &header, sizeof(headerIPC_t));
        if(status != ERROR)
        {
            block = GetBlock(data, header.size, npacket-1);
            status = write(writeFifo_FD,block, header.size);
            free(block);
            
            if( status == ERROR )
            {
                pthread_mutex_unlock(&wrmtx);
                return ERROR;
            }
            
        }
        
        bytesLeft -= PACKET_SIZE;
        npacket++;
    }
    printf("Mande %d paquetes por %s \n", header.totalPackets, writeFifo);
    fflush(stdout);
    pthread_mutex_unlock(&wrmtx);
    return status;
}

byte *
ReadIPC(key_t key)
{
    pthread_mutex_lock(&rdmtx);
    InitReadIPC(key);
    
    int status = OK;
    headerIPC_t header;
    byte * data=NULL;
    int nPacketsRead;
    int pos;
    byte *aux;
    
    printf("\nEscuchando pedidos...\n");
    fflush(stdout); 
    nPacketsRead=0;
    pos=0;
    do{
        status = read(readFifo_FD, &header, sizeof(headerIPC_t));

        if(status > 0 && header.size > 0)
        {
            data = realloc(data, pos + header.size );
            aux = malloc(header.size);          
            status=read(readFifo_FD, aux, header.size);

            memmove(data+pos, aux, header.size);
            pos += header.size;
            nPacketsRead++;
                
            if(status > 0)
            {
                status = OK;
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
            pthread_mutex_unlock(&rdmtx);
            return NULL;
            
        }
    }while( status != ERROR && nPacketsRead < header.totalPackets );

    printf("Recibidos: %d\n", nPacketsRead);
    fflush(stdout);
    pthread_mutex_unlock(&rdmtx);
    return status == ERROR ? NULL: data ;
}

void
CloseIPC(void)
{
    /*Liberar recursos...*/
}

static byte *
GetBlock(byte *org, size_t size, int index)
{
    byte *block;
    
    if( (block=malloc(size)) == NULL ) {
        return NULL;
    }
    
    return memmove(block, org+index*size, size);
}



