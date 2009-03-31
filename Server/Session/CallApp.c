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

/* New Client */

session_t
FillPack(string senderID, string msg, uInt opCode, 
				pid_t pid, size_t dataSize, byte *data)
{
	session_t aux;
	
	if( msg == NULL && strlen(msg) > MAXMSG )
		strcpy(aux.msg,"");
	else	
		strcpy(aux.msg,msg);
	
	if( senderID == NULL && strlen(senderID) > MAXSENDER )
		strcpy(aux.senderID,"");
	else	
		strcpy(aux.senderID,senderID);
	
	aux.opCode   = opCode;
	aux.pid      = pid;
	aux.dataSize = dataSize;
	aux.data     = data;
	
	return aux;
}

int
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
			opCode = ERROR;
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

/* UserList */

byte *
MakeMemBlockUsrList(string *usrLst, int nusr)
{
	byte *memblock;
	int pos;
	int i;
	
	/* Almaceno un bloque continuo de memoria para guardar todos los arreglos */
	if( (memblock = malloc(sizeof(string) * (MAX_USR_NAME+1) * nusr )) == NULL ) {
		return NULL;
	}
	
	/*Copio uno por uno los strings, de manera continua sobre el bloque */
	pos = 0;
	for( i=0; i < nusr; i++ ) {
		memmove(memblock + pos, usrLst[i], strlen(usrLst[i])+1 );
		pos += MAX_USR_NAME+1;
	}
	
	return memblock;
}

int
MakeUserListPack(string *usrLst, int nusr, session_t *dataPtr)
{
	session_t aux;
	int dataSize;
	byte *data;
	
	dataSize = nusr*sizeof(string)*(MAX_USR_NAME+1);
	if(  (data = MakeMemBlockUsrList(usrLst, nusr)) == NULL ) {
		return ERROR;
	}
	
	aux = FillPack( NULL, NULL, (*dataPtr).opCode, 0, dataSize, data );
	*dataPtr = aux;	
	
	return OK;
}

int 
CallUserList(session_t *dataPtr)
{
	string *usrLst;
	int nusr;
	
	if( (nusr=UserList(&usrLst)) == ERROR ) {
		return ERROR;
	}
	
	return MakeUserListPack(usrLst, nusr, dataPtr);
}	


/*
 static int CallTopList(session_t data);
 static int CallTopListUser(session_t data);
 static int CallDirReq(session_t data);
 static int CallDirRem(session_t data);
 static int CallFileAdd(session_t data);
 static int CallFileRem(session_t data);
 static int CallFileAdd(session_t data);
 static int CallDirList(session_t data);
 */
