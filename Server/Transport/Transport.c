#include "Transport.h"
#include "TransportDefines.h"
#include "ipcInterface.h"

typedef struct {
	int dni;
	int esgay;
}cacaT;

int
InitTransport(void)
{
    return InitMainIPC();
}

void
Listen(void)
{
    int status;
    pid_t pid;
    char  data;
    status = InitIPC(0);
    while(TRUE)
    {
        pid = ReadRequest(&data);
        /* se manda a que sea procesado */
        ProcessRequest(&data, pid);
    };
    return;
}

pid_t
ReadRequest(void* data)
{
    int requestExists = FALSE;
    pid_t pid;
    while(!requestExists)
    {
        if( (pid = ReadIPC(data)) != -1)
        {
            requestExists = TRUE;
        }
    }
	
    return pid;
}

void 
ProcessRequest(void * data, pid_t requestPid)
{
	/*	printf("llego un mensaje!: %d - %s es gay \n", ((cacaT*)data)->dni,((cacaT*)data)->esgay?"SI":"NO");

    pid_t childProcess;

    childProcess = fork();
	
    switch(childProcess)
    {
        case -1: 
                fprintf(stderr,"Se ha producido un error en el servidor\n");
           	exit(EXIT_FAILURE);
                break;
        case 0:
                InitChildProcess(__DEFAULT_CHILD_PID__);
           /*     ListenToChild(data, requestPid);*/
      /*          break;
        default:
                printf("soy el proceso padre...mando request a sesion");
                /*aca llamar a sesion,que interpreta data y llama a la funcion
                    de aplicacion que sea necesario*/
    /*}*/
}

void
InitChildProcess(pid_t pid)
{
    InitIPC(pid);
}
