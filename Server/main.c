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
#include "./Transport/OutputPipe.h"

int
main(void)
{
    int status;

    status = InitPromptCommunication();
    status = InitTransport();
    status = InitApplication();
	status = InitServerPath();
	
    if(status != ERROR)
    {
		status = StartListening();
		if(status == ERROR)
		{
			printf("Se ha producido un error\n");
		}
    }
    else
    {
		fprintf(stderr, "No es posible inicializar la aplicacion.\n");
    }

    CloseApplication();

    return 0;
}	
