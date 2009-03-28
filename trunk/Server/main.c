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
    InitApplication();
    //StartListening();
	
	
	session_t aux;
	
	aux.senderID[0] = 'H';
	aux.senderID[1] = '\0';
	
	aux.msg[0] = '\0';	
	aux.opCode = PR_DIR_REG;
	aux.pid = 3;
	aux.dataSize = 50;
	
	ProcessRequest(MakeSessionData(aux), 4);
	
	
    return 0;
}	
