/*
 *  client.c
 *  tpSO1
 *
 *  Created by Damian Dome on 3/15/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "client.h"



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
InitClientApp()
{
	int aux;
	fileT *fileList;
	string *auxArray;
	
	fileList = InitFiles();
	
	switch(fork()) {
		case 0: 
			aux = FilesWatch(auxArray=MakeFilesArray(fileList, 5), 5);
			
			printf("Cambio un archivo %d\n", aux);
			
			free(auxArray);
			
			return OK;
		default:
			ClientPrompt();
			wait(NULL);
	}		
	
	free(fileList);
	return OK;
}

int
ClientPrompt()
{
	return OK;
}

fileT *
InitFiles()
{
	int i;
	
	//Por ahora tenemos 5 fijos
	//creados en ejecucion
	//deberiamos levantarlos al iniciar, de alguna manera.
	fileT *array;	
	
	array = malloc(sizeof(fileT)*5);
	
	for( i=0; i<5; i++ ) {
		array[i].idFile = i;
		array[i].path = "/SO/";
	}
	array[0].fName = "Prueba1";
	array[1].fName = "Prueba2";
	array[2].fName = "Prueba3";
	array[3].fName = "Prueba4";
	array[4].fName = "Prueba5";		
	for( i=0; i<5; i++ ) {
		CreateFile(array[i].path, array[i].fName);
	}
	
	return array;		
}