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
    /*status = InitTransport();
    if(status != ERROR)
    {
		status = StartListening();
    }
    else
    {
		fprintf(stderr, "No es posible inicializar la aplicacion.\n");
    }*/
Prompt();
	 
/*	//InitApplication();
	//Prompt();
	
	session_t aux;
	string dirpath;
	int pos;
	byte *fileData;
	byte *sessionData;
	
	strcpy(aux.msg,"Damian");
	aux.pid = 31;
	aux.opCode = CL_DIR_REQ;
	
	aux.dataSize = strlen("/SO")+1;		
	aux.data = malloc(aux.dataSize);
	strcpy(aux.data,"/SO");
		
	sessionData = MakeSessionData(aux);
	ProcessRequest(&sessionData, 31); */

    CloseApplication();

    return 0;
}	
