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

/* Server -> Client: FileAdd/FileMod/FileRem */

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
	int ret;

	GetFileData(data,&file,fileData);		
	ret = FileAdd(file,fileData);	
	
	free(data.data);
	return ret;
}	

int 
CallFileMod(session_t data)
{
	fileT file;
	byte *fileData;
	int ret;

	GetFileData(data,&file,fileData);	
	
	/*Lo saco y vuelvo a insertar */
	FileRem(file);	
	ret = FileAdd(file,fileData);	
	
	return ret;	
}	

int 
CallFileRem(session_t data)
{
	fileT file;
	int ret;
	
	GetFileData(data,&file,NULL);	
	
	ret = FileRem(file);	
	
	return ret;
}


/* Server -> Client: Agregar directorio */

int
GetDirData(byte *data, fileT **fileListPtr, byte ***dataBufferPtr)
{
	int nfiles;
	int pos;
	int i;
	
	fileT *fileList;
	byte **dataBuffer;
	
	pos =0;
	memmove(&nfiles, data, sizeof(int));
	
	fileList = malloc(sizeof(fileT)*nfiles);
	dataBuffer = malloc(sizeof(byte *)*nfiles);
	
	pos+=sizeof(int);
	for( i=0; i<nfiles; i++ ) {
		memmove(&(fileList[i]), data + pos, sizeof(fileT));
		pos += sizeof(fileT);
		dataBuffer[i] = malloc(GetSize(fileList[i]));
		memmove(&(dataBuffer[i]), data + pos,  GetSize(fileList[i]));
		pos += GetSize(fileList[i]);
	}
	
	
	*fileListPtr = fileList;
	*dataBufferPtr = dataBuffer;
	
	return nfiles;
}

int 
CallDirAdd(session_t data)
{
	fileT *fileList;
	byte **dataBuffer;
	int nfiles;
	
	nfiles = GetDirData(data.data, &fileList, &dataBuffer);

	return DirAdd(data.msg, fileList, dataBuffer, nfiles);	
}	

/* Server -> Client: Listar directorios */

int
GetDirList(session_t data, int *ndirs, string **dirList)
{
	int pos;
	int i;
		
	pos = 0;
	memmove(ndirs, data.data + pos, sizeof(int));
	pos += sizeof(int);
	
	if( (*dirList=malloc((*ndirs)*MAX_DIR_NAME)) == NULL ) {
		return ERROR;
	}
	
	for(i=0; i<(*ndirs); i++) {
		(*dirList)[i] = malloc(MAX_DIR_NAME);
		memmove(((*dirList)[i]), data.data+pos, MAX_DIR_NAME);		
		pos += MAX_DIR_NAME; 
	}
	
	
	return OK;
}

int
CallDirList(session_t data)
{
	int ndirs;
	string *dirList;
	int i;
	
	GetDirList(data,&ndirs,&dirList);
	for(i=0; i<ndirs; i++) {
		printf("%s\n", dirList[i]);
	}
	getchar();

	return OK;
}
		
		
		
		
	
	
	










