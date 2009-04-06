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
		if(status == ERROR)
		{
			printf("Se ha producido un error\n");
		}
    }
    else
    {
		fprintf(stderr, "No es posible inicializar la aplicacion.\n");
    } 
	 
/*	InitApplication();
	printf("%d\n",NewClient(1));
	printf("%d\n",NewClient(2));
	/*printf("%d\n",NewClient(1));*/
/*	printf("%d\n",NewClient(3));
	printf("%d\n",NewClient(4));
	printf("%d\n",NewClient(5));
	
	printf("Conectando...\n");
	printf("%d\n",ConnectUser(1,"puto"));
	printf("%d\n",ConnectUser(2,"puto"));
	printf("%d\n",ConnectUser(11345,"puertgto"));*/

    CloseApplication();

    return 0;
}	
