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
#include "../Transport/OutputPipe.h"

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
GetFileData(session_t data,fileT *filePtr, byte **fileData_ptr)
{
	int pos;
	byte *fileData;
	
	printf("Leo la info\n");
    fflush(stdout);
      
	pos=0;
	memmove(filePtr, data.data + pos, sizeof(fileT) );
	
	fileData = malloc(GetSize(*filePtr));
	
	pos += sizeof(fileT);
	memmove(fileData, data.data + pos, GetSize(*filePtr) );
	
	*fileData_ptr = fileData;	
		
}	

int 
CallFileTransfer(session_t data)
{
	fileT file;
	byte *fileData;
	int ret;
	
	GetFileData(data,&file,&fileData);


    if( FileExists(file) ) {
	    FileRem(file);    	
    }
	ret = FileAdd(file,fileData);

	//free(data.data);
	//free(fileData);
	
	return ret;
}	



int 
CallFileRem(session_t data)
{
	fileT *file;
	int ret;
	
	file = (fileT *)(data.data);
	
	ret = FileRem(*file);	
	
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
	pos+=sizeof(int);
	
	fileList = malloc(sizeof(fileT)*nfiles);
	dataBuffer = malloc(sizeof(byte *)*nfiles);
	

	for( i=0; i<nfiles; i++ ) {
		memmove(&(fileList[i]), data + pos, sizeof(fileT));
		pos += sizeof(fileT);
		
		if( (dataBuffer[i] = malloc(GetSize(fileList[i]))) == NULL ) {
			return ERROR;
		}
		memmove( dataBuffer[i], data + pos,  GetSize(fileList[i]));
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
	printf("==========================\n");
	printf("Directorios disponibles\n");
	printf("==========================\n");
	for(i=0; i<ndirs; i++) {
		printf("%s\n", dirList[i]);
	}
	printf("\n");
	fflush(stdout);

	return OK;
}
		
int 
CallDirDel(session_t data)
{
    

    printf("--%s--\n",data.msg);
    fflush(stdout);
    
    rmdir(Concat(Concat(data.senderID,"/"),data.msg));
    return OK;
}		
		
int 
CallDirNew(session_t data)
{

    printf("--%s--\n",data.msg);
    fflush(stdout);
    
    mkdir(Concat(Concat(data.senderID,"/"),data.msg),0777);
    return OK;
}			
	
	
	










