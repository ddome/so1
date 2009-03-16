/*
 *  client.h
 *  tpSO1
 *
 *  Created by Damian Dome on 3/15/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <unistd.h>
#include "fileHandler.h"
#include "../Lib/genlib.h"
#include "../Lib/defines.h"

typedef struct {
	int idFile;
	string path;
	string fName;
} fileT;


fileT * InitFiles(void);
int ClientPrompt(void);
int InitClientApp(void);

#endif