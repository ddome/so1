/*
 *  CallApp.h
 *  server
 *
 *  Created by Damian Dome on 3/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CALL_APP_
#define _CALL_APP_

int CallNewClient(session_t *dataPtr);

int CallUserList(session_t *dataPtr);

int CallDirRem(session_t data);

int CallFileAdd(session_t data);

int CallFileMod(session_t data);

int CallFileRem(session_t data);

int CallFileRem(session_t data);

int CallDirReg(session_t data);

#endif