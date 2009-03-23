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
#include <dirent.h>
#include "../Lib/defines.h"
#include "../Lib/genlib.h"


typedef struct {
	int idFile;
	string path;
	string fName;
	struct stat sb; //Guardo el estado inicial del archivos
} fileT;

// Funciones de control de permisos. No estan implementadas
boolean UserCanRead( string path );
boolean GroupCanRead( string path );
boolean AnyCanRead( string path );
boolean UserCanWrite( string path );
boolean GroupCanWrite( string path );
boolean AnyCanWrite( string path );

fileT NewFileT( string path, string fName );

int FilesWatch( string *path, int nfiles );

int FilesHasChanged( fileT *files, int nfile );

FILE * CreateFile( fileT file );

boolean FileExists( fileT file );

int InitFilesStat(fileT *files, int nfile);

int DeleteFile( fileT file );

int CopyFile( string srcFile, string destFile );

int CopyDir( string srcDir, string destDir );

fileT NewFileT( string path, string fName );

FILE * OpenFile( fileT file );

int GetSize(fileT file);

static string GetPath( fileT file );

FILE *OpenReadFile( fileT file );

#endif