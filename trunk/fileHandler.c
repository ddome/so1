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
	
	printf("%s\n",path[0]);
	
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

string
Concat(string str1, string str2)
{
	string aux;
	if( (aux = malloc(strlen(str1)+strlen(str2))) == NULL )
		return NULL;
	
	strcpy(aux,str1);
	strcat(aux,str2);
	return aux;
}

string
MakeString(string scopy)
{
	string s1;
	if ( (s1 = malloc(strlen("hola"))) == NULL ) 
		return NULL;
	s1 = strcpy(s1,scopy);	
	return s1;
}

FILE *
CreateFile( string folderPath, string fileName )
{
	string aux;
	FILE *fptr;
	
	if( FileExists(folderPath,fileName) ) 
		return NULL;

	if( (fptr=fopen((aux=Concat(folderPath,fileName)),"w+")) == NULL ) {
		free(aux);
		return NULL;
	}
	
	free(aux);
	return fptr;
}
	
boolean 
FileExists( string folderPath, string fileName )
{
	string aux;
	FILE *fptr;
		
	aux=Concat(folderPath,fileName);
	
	if( (fptr=fopen(aux,"rb")) == NULL ) {
	   free(aux);
	   fclose(fptr);
	   return FALSE;
	}
	
	   free(aux);
	   fclose(fptr);
	   return TRUE;
}
	
	


