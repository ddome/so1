/*
 *  CallApp.c
 *  server
 *
 *  Created by Damian Dome on 3/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#include "Session.h"
#include "CallApp.h"
#include "../App/fileHandler.h"

/* Funciones generales */

static session_t
FillPack(string senderID, string msg, uInt opCode, 
		 pid_t pid, size_t dataSize, byte *data)
{
	session_t aux;
	
	if( msg == NULL || strlen(msg) > MAXMSG )
		strcpy(aux.msg,"");
	else	
		strcpy(aux.msg,msg);
	
	if( senderID == NULL || strlen(senderID) > MAXSENDER )
		strcpy(aux.senderID,"");
	else	
		strcpy(aux.senderID,senderID);
	
	aux.opCode   = opCode;
	aux.pid      = pid;
	aux.dataSize = dataSize;
	aux.data     = data;
	
	return aux;
}


/* Client -> Server: New Client */

static int
MakeNewClientRetPack(int op, session_t *dataPtr )
{
	session_t aux;
	int ret;
	uInt opCode;
	
	switch(op) {
		case NEW_USR_EXIST: 
			opCode = SR_NEW_USR_ERR;
			ret = OK;
			break;
			
		case NEW_USR_OK:
			opCode = SR_NEW_USR_OK;
			ret = OK;
			break;
			
		default:
			opCode = NEW_USR_ERROR;
			ret = ERROR;
			break;
	}
		
	aux = FillPack(NULL,NULL, opCode, 0, 0, NULL );
	
	/* Guardo la respuesta del Server */
	*dataPtr = aux;
	return ret;		
}

int 
CallNewClient(session_t *dataPtr)
{	
	int ret;
	
	ret=NewClient((*dataPtr).msg,(*dataPtr).pid);	
	return (MakeNewClientRetPack(ret, dataPtr));
}	

/* Client -> Server: DirRem */	

static string
GetDirName(session_t data)
{
	if( data.dataSize == 0 ) {
		return NULL;
	}
	else {
		return data.data;
	}
}

int 
CallDirRem(session_t data)
{
	string dir;
	string userName;

	dir = GetDirName(data);
	userName = data.msg;

	return DelDir(userName,dir);
}

/* Client -> Server: FileAdd/FileMod/FileRem */

void
GetFileData(session_t data,fileT *filePtr, byte *fileData)
{
	int pos;
	
	pos=0;
	memmove(filePtr, data.data + pos, sizeof(fileT) );
	
	pos += sizeof(fileT);
	if( (data.dataSize - sizeof(fileT) ) > 0 && fileData != NULL ) {
		memmove(fileData, data.data + pos, (data.dataSize - sizeof(fileT)) );	
	}	
}	

int 
CallFileAdd(session_t data)
{
	fileT file;
	byte *fileData;
	string user;  // Usado solo para agregar a los Logs
	
	user = data.msg;

	GetFileData(data,&file,fileData);	
	LogAction(user, GetPath(file), "Add");
	
	return FileAdd(file,fileData);	
}	

int 
CallFileMod(session_t data)
{
	fileT file;
	byte *fileData;
	string user; // Usado solo para agregar a los Logs
	
	user = data.msg;
	GetFileData(data,&file,fileData);	
	LogAction(user, GetPath(file), "Mod");
		
	/*Lo saco y vuelvo a insertar */
	FileRem(file);	
	return FileAdd(file,fileData);
}	

int 
CallFileRem(session_t data)
{
	fileT file;
	string user; //Usado solo para agregar a los Logs
	
	user = data.msg;	
	GetFileData(data,&file,NULL);
	LogAction(user, GetPath(file), "Del");
	
	return FileRem(file);	
}




/*

 static int CallTopList(session_t data);
 static int CallTopListUser(session_t data);
 static int CallDirReq(session_t data);
 static int CallDirList(session_t data);
 */ 


