/*
 *	System includes
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/*
 *	Proyect includes
 */
#include "Lib/scannerADT.h"
#include "Lib/tree.h"
#include "App/Application.h"
#include "Transport/Transport.h"

#define TRUE 1
#define FALSE 0
#define OK 1

#define MAX_LINE 120

int main( void )
{
	int status;
	InitApplication();
	status = InitTransport();
	
    if(status == ERROR )
    {
        fprintf(stderr,"No es posible iniciar el servidor.\n");
        exit(EXIT_FAILURE);
    }
	
	Listen();	
    return 0;
}


