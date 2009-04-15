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

int
main(void)
{	
    int status;
    status = InitTransport();
    status = InitApplication();
    InitPromptCommunication(getpid());
    WritePrompt("afgafg\n");
    printf("%s\n",ReadMessage());


    if(status != ERROR)
    {
        status = StartListening();
		if(status == ERROR)
		{
		    printf("ERROR");
		    getchar();
		}
    }
    else
    {
        fprintf(stderr, "No se ha podido inicializar la aplicacion.");
    }
    return 0;
}	

