/* Archivo: main.c
*  ---------------
*  Punto de inicializacion y entrada del programa.
*/


#include <stdio.h>
#include "./App/Application.h"
#include "./App/Server.h"
#include "./Transport/Transport.h"
#include "./Lib/defines.h"
int
main(void)
{	
    int status;
    status = InitApplication();
    status = InitTransport();
    if(status != ERROR)
    {
        status = StartListening();
    }
    else
    {
        fprintf(stderr, "No se ha podido inicializar la aplicacion.");
    }
    
	return 0;
}	
