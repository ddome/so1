/*
 *  main.c
 *  server
 *
 *  Created by Damian Dome on 3/25/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#include <stdio.h>
#include "./App/Server.h"
#include "./App/Application.h"
#include "./Session/Session.h"
#include "./Transport/Transport.h"
#include "./sqliteADT/sqliteADT.h"

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
		fprintf(stderr, "No es posible inicializar la aplicacion.\n");
    }
	 
	/*InitApplication();
	
	session_t aux;
	fileT filein,fileout;
	int pos;
	byte *fileData;
	byte *sessionData;
	
	strcpy(aux.msg,"Damian");
	aux.pid = 31;
	aux.opCode = CL_FIL_REM;
	
	filein = NewFileT("/SO","Prueba1");
	fileData = ReqFile(filein);
	
	fileout = NewFileT("/OUT","Prueba1");
	fileout.sb.st_size = filein.sb.st_size;
	
	
	aux.dataSize = GetSize(fileout) + sizeof(fileT);		
	aux.data = malloc(aux.dataSize);	
	pos = 0;	
	memmove(aux.data+pos, &fileout, sizeof(fileT));
	pos += sizeof(fileT);
	memmove(aux.data+pos, fileData, GetSize(fileout));

	sessionData = MakeSessionData(aux);
	ProcessRequest(&sessionData, 31);*/

    return 0;
}	
