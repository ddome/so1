/*
 *  fileHandler.c
 *  
 *
 *  Created by Damian Dome on 3/11/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "fileHandler.h"


static string *
MakeFilesArray(fileT *files, int nfiles)
{
	string *array;
	int i;
	
	if( (array=malloc(sizeof(string)*nfiles)) == NULL )
		return NULL;
	
	for( i=0; i<nfiles; i++ ) {
		array[i] = Concat(files[i].path,files[i].fName);
	}
	
	return array;
}

int
FilesWatch( string *path, int nfile )
{
	int i;
	struct stat sb;
	time_t last_time[MFILE+1];
	
	//Inicializacion
	for ( i=0; i < nfile; i++ ) {
		
		if( stat(path[i],&sb) == -1 )
			return ERROR;
		
		last_time[i] = sb.st_mtime;
	}	
	
	
	//Loop que controla cada uno de los archivos
	//El ciclo termina cuando cambia un archivo
	for(;;) {
        
		for( i=0; i < nfile; i++ ) {
			if( stat(path[i],&sb) == -1 || sb.st_mtime != last_time[i] )
				return i;
				
			//Espera para volver a controlar
			sleep(SLEEP_TIME);
			
			printf("Desperto\n");
		}
	}	
}

int
InitFilesStat(fileT *files, int nfile)
{
	int i;
	string aux;
	
	
	for ( i=0; i < nfile; i++ ) {
			
		aux = Concat(files[i].path, files[i].fName);
		if( stat(aux,&(files[i].sb)) == -1 )
			return ERROR;
		free(aux);
	}
	return OK;
}
	
int
FilesHasChanged( fileT *files, int nfile )
{
	int i;
	string aux;
	struct stat sb;
	
	
	for( i=0; i < nfile; i++ ) {
		aux = Concat(files[i].path, files[i].fName);
		if( stat(aux,&sb) == -1 || sb.st_mtime != files[i].sb.st_mtime ) {
			files[i].sb = sb;
			return i;
		}
		free(aux);
	}
	return -1;
}


FILE *
CreateFile( string folderPath, string fileName )
{
	string fName;
	FILE *fptr;
	
	if( FileExists(folderPath,fileName) == TRUE )
		return NULL;
		
	if( (fName=Concat(folderPath,fileName)) == NULL )
	   return NULL;
	
	if( (fptr=fopen(fName,"wb+")) == NULL ) {
		free(fName);
		return NULL;
	}
	
	return fptr;
}
	   
boolean 
FileExists( string folderPath, string fileName )
{
	string fName;
	FILE * fptr;
	
	if( (fName=Concat(folderPath,fileName)) == NULL )
		return FALSE;
	
	if( (fptr=fopen(fName,"r")) == NULL ) {
		free(fName);
		return FALSE;
	}
	free(fName);
	fclose(fptr);
	return TRUE;
}


