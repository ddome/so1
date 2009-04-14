


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

    sprintf(path,"%s%d_wr",COMM_DIR,(int)key);

    return path;
}

string
MakeWRPath(key_t key)
{
    string path;
    if((path = calloc(MAX_DIR_NAME, sizeof(char)))==NULL)
	return NULL;

    sprintf(path,"%s%d_rd",COMM_DIR,(int)key);

    return path;
}


/* Funcion: InitIPC()
*  ------------------
*  Crea un ipc para comunicarse con el proceso univocamente 
*  representado por key
*/
int InitIPC(key_t key)
{
  /* Se arman los nombres de los fifos para el proceso que se pidio, 
  *  y luego se crean los fifo's propiamente.
  */
  readFifo=MakeRDPath(key);
  writeFifo=MakeWRPath(key);


	
  writeFifo_FD = open(writeFifo, O_RDWR );
  readFifo_FD = open(readFifo, O_RDWR  );

  if(readFifo_FD == -1 || writeFifo_FD == -1 )
  {
    printf("no se puede abrir el fifo\n");
  }
   
  IPCStarted = TRUE;
  isChildProcess = TRUE;

  return (writeFifo_FD > 1 && readFifo_FD>1 )? OK: ERROR ;
}

int 
WriteIPC(void * data, size_t size)
{
    int status;
    /* Se arma el header de transporte
    */
    headerIPC_t header;
    header.nPacket = 1;
    header.size = size;
    status = write(writeFifo_FD, &header, sizeof(headerIPC_t));
    if(status != ERROR)
    {
        status = write(writeFifo_FD,data,size);

    }
    return status;
}

byte *
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

  return status == ERROR ? NULL: data ;
}

void
CloseIPC(void)
{
    /*Liberar recursos...*/
}

