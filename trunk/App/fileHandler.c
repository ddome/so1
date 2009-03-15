/*
 *  fileHandler.c
 *  
 *
 *  Created by Damian Dome on 3/11/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "fileHandler.h"

int
FilesWatch( string *path, int nfile )
{
	int flag;
	int i,j;
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
		}
	}	
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
	
	
int
main(void)
{
	string folder;
	string name;
	
	folder = CreateString("/Users/damian/");
	name   = CreateString("prueba.txt");
	
	printf("%d\n", FileExists(folder,name) );
	
	CreateFile(folder,name);
	
	printf("%d\n", FileExists(folder,name) );
	
}


