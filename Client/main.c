/* Archivo: main.c
*  ---------------
*  Punto de inicializacion y entrada del programa.
*/


#include <stdio.h>
#include "./App/Application.h"
#include "./App/Server.h"
#include "./Transport/Transport.h"
#include "./Lib/defines.h"
#include "./Transport/OutputPipe.h"

int SendDirPack(process_t process);

int
main(void)
{	
    int status[6];
    int i;

    status[0] =  InitTransport();
    status[1] =  InitApplication();
    status[2] =  InitServerPath();
    status[3] =  InitNotify();
    status[4] =  InitPromptCommunication(getpid());

    for( i=0; i<5; i++ ) {
	if( status[i] == ERROR ) {
		WritePrompt("No se a podido inicializar la aplicacion");
		return 0;
	}
    }

    status[5] = StartListening();
    if(status[5] == ERROR) {
	WritePrompt("No se a podido inicializar la comunicacion");
    }
   
    return 0;
}


