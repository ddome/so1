/*
 *  CallApp.h
 *  server
 *
 *  Created by Damian Dome on 3/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __CALLAPP_H__
#define __CALLAPP_H__

int CallFileAdd(session_t data);

int CallFileMod(session_t data);

int CallFileRem(session_t data);

int CallDirAdd(session_t data);

int CallDirList(session_t data);

int CallFileTransfer(session_t data);

int CallDirDel(session_t data);

int CallDirNew(session_t data);

#endif
