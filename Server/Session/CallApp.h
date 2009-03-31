/*
 *  CallApp.h
 *  server
 *
 *  Created by Damian Dome on 3/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

session_t FillPack(string senderID, string msg, 
				   uInt opCode, pid_t pid, size_t dataSize, byte *data);

int MakeNewClientRetPack(int op, session_t *dataPtr );

int CallNewClient(session_t *dataPtr);

byte * MakeMemBlockUsrList(string *usrLst, int nusr);

int MakeUserListPack(string *usrLst, int nusr, session_t *dataPtr);

int CallUserList(session_t *dataPtr);