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



int CallDirList(session_t data);


int
MakeDirListPack( int ndirs, string *dirList, byte **dataBuffer )
{
	int pos;
	int size;
	int i;
	
	*dataBuffer = malloc(size=ndirs*MAX_DIR_NAME+sizeof(int));
	
	pos = 0;
	memmove(*dataBuffer, &ndirs, sizeof(int));
	pos += sizeof(int);
	
	for( i=0; i<ndirs; i++ ) {
		memmove(*dataBuffer+pos, &(dirList[i]), MAX_DIR_NAME);
		pos += MAX_DIR_NAME;
	}
	
	return size;
}


int
main(void)
{	
    int status;
    status = InitApplication();
	status = InitPromptCommunication();
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

