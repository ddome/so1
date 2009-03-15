/*
 *  fileHandler.h
 *  
 *
 *  Created by Damian Dome on 3/15/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FILEHANDLER_H_
#define _FILE

#define MFILE 15
#define SLEEP_TIME 60


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "defines.h"
#include <string.h>
#include <stdlib.h>


//Monitorea un arreglo de archivos hasta que uno sea modificado. 
//Devuelve -1 si se produjo un error. 
//Al modificarse un archivo vigilado, la funcion
//finaliza y se devuelve el index del archivo modificado dentro del array.
int FilesWatch( string *path, int cant );

//Devuelve NULL si ya existe o no pudo crearse.
FILE* CreateFile( string folderPath, string fileName );

boolean FileExists( string folderPath, string fileName );

//Funciones de control de permisos, no estan implementadas
boolean UserCanRead( string path );
boolean GroupCanRead( string path );
boolean AnyCanRead( string path );
boolean UserCanWrite( string path );
boolean GroupCanWrite( string path );
boolean AnyCanWrite( string path );

#endif