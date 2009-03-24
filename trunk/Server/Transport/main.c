#include "Transport.h"

#define ERROR -1
#define OK     1

int main(void)
{
    int status;
    /* Se inicializan las 3 capas del programa
    *  Si ninguna falla, se comienza a escuchar pedidos internos 
    *  o del cliente
    */
    status = InitTransport();

    if(status == ERROR )
    {
        fprintf(stderr,"No es posible iniciar el servidor.\n");
        exit(EXIT_FAILURE);
    }
 /*    status = InitSession();

   if(status == ERROR )
    {
        fprintf(stderr,"No es posible iniciar el servidor.\n");
        exit(EXIT_FAILURE);
    }
    status = InitApplication();

    if(status == ERROR )
    {
        fprintf(stderr,"No es posible iniciar el servidor.\n");
        exit(EXIT_FAILURE);
    }*/

    Listen();

    return OK;
}
