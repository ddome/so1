
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Prompt.h"
#include "../Lib/genlib.h"
#include "../Lib/defines.h"
#include "fileHandler.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_PATH "backup/"
#include "../Lib/pqADT.h"


/*---------------------------------------------------------------- 
 Inicia la aplicación. Requerido antes de usar cualquier función
 de la misma.
 ----------------------------------------------------------------*/
int InitApplication(void);

/*---------------------------------------------------------------- 
 Agrega un directorio 'dir', que contiene la lista de archivos
 'filesList, con su contenido binario almacenado en la posición
 correspondiente de data, con 'nfiles' elementos almacenados.
 ----------------------------------------------------------------*/
int DirAdd( string dirName, fileT *files, byte **data, int nfiles  );

/*---------------------------------------------------------------- 
 Borra un archivo del cliente
 ----------------------------------------------------------------*/
int FileRem( fileT file );

/*---------------------------------------------------------------- 
 Agrega un archivo al cliente. Recibe en data el arreglo de bytes
 con el contenido binario del archivo. El usuario es responsable
 de liberar data.
 ----------------------------------------------------------------*/
int FileAdd( fileT file, byte *data );

/*---------------------------------------------------------------- 
 Almacena en 'data' el contenido binario de un archivo indicado
 por 'file'. El usuario es responsable de liberar data.
 ----------------------------------------------------------------*/
byte  *FileReq( fileT file );

/*---------------------------------------------------------------- 
 Vigila un directorio hasta que haya un cambio, devolviendo
 un descriptor con el tipo de operación sobre el directorio.
 ----------------------------------------------------------------*/
int DirWatch(string dir);

/* debugueo */
byte *
ReqFile( fileT file );
int
ReqDir( string userName, string dirName, fileT **files, byte ***databuffer );


#endif
