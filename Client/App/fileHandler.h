/*
 *  fileHandler.h
 *  
 *
 *  Created by Damian Dome on 3/15/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FILEHANDLER_
#define _FILEHANDLER_

#define MFILE 15
#define MPATHL 100 //maximo tamanio del path de un file
#define SLEEP_TIME 10


#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Lib/defines.h"
#include "../Lib/genlib.h"


// Funciones de control de permisos. No estan implementadas
boolean UserCanRead( string path );
boolean GroupCanRead( string path );
boolean AnyCanRead( string path );
boolean UserCanWrite( string path );
boolean GroupCanWrite( string path );
boolean AnyCanWrite( string path );


// Vigila archivos almacenados en path, hasta que cambie
// alguno, devolviendo su indice dentro del arreglo.
// si no existe algun archivo, devuelve ERROR
int FilesWatch( string *path, int nfiles );

int FilesHasChanged( string *path, int nfile );

FILE * CreateFile( string folderPath, string fileName );

boolean FileExists( string folderPath, string fileName );

#endif