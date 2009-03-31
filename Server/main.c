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
    printf("%d\n",NewClient("hola",1));
    printf("%d\n",NewClient("hola2",1));
    printf("%d\n",NewClient("hola",2));
    printf("%d\n",NewClient("hola",1));
    DirconnectUser("hola");
    
    
    printf("%d\n",NewClient("hola",1));
    /*printf("%d\n",NewClient("hola",1));
    printf("%d\n",NewClient("hola",1));*/
    
    
    
    return 0;
}	
