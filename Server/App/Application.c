#include "Application.h"
#include "Prompt.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int entry=0;
long int i = 0;
int a=0,b=0,c=0;

void
ServeClient(int d)
{
    int prevA=d;
    while(i < 24000000)
    { 
        i++;
        if(i == 20000000)
            printf("El cliente ha cambiado un archivo\n");
    }
}

void
StartListening(void)
{
    int prevEntry=0, hasChild=0;
    pid_t pid = 1;
    
    while(i < 25000000)
    {
        if( !hasChild || pid>0 )
        {
            if(i==10000000)
                entry=1;
            if(entry!=prevEntry)
            {
                printf("Se ha cambiado el valor de entry.\n");
                prevEntry=entry;
                hasChild=1;
                pid=fork();
            }
            i++;
        }
        if( hasChild && pid==0 )
        {
            ServeClient(a); 
        }
        hasChild=0;
    }

}

void
StartApplication(void)
{
    pid_t pid;
    pid=fork();
    if(pid==0)
        StartListening();
    else if(pid>0)
        Prompt();
    else
        fprintf(stderr,"Error en el fork()\n");
}

