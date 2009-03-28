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

int
main(void)
{
    int status;
    printf("%d\n",InitTransport());
    status = InitApplication();
    if(status != ERROR)
    {
	status = StartListening();
    }
    else
    {
	fprintf(stderr, "No es posible inicializar la aplicacion.\n");
    }
          
    return 0;
}	
