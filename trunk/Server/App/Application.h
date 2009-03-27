
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Prompt.h"
#include "../Lib/genlib.h"
#include "../Lib/defines.h"
#include "fileHandler.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_PATH "/bomsyinc/backup"
#include "../sqliteADT/sqliteADT.h"
#include "../Lib/pqADT.h"
#include "DBexec.h"


/*---------------------------------------------------------------- 
 Inicia la aplicación. Requerido antes de usar cualquier función
 de la misma.
 ----------------------------------------------------------------*/
int InitApplication(void);

/*---------------------------------------------------------------- 
 Pregunta por el cliente. En caso de ser existente, lo loguea.
 en caso de no existir, lo agrega a la base. En caso de existir y 
 estar conectado, devuelve un error.
 ----------------------------------------------------------------*/
int NewClient(string name,int pid);

/*---------------------------------------------------------------- 
 Recibe un nombre de usuario y un puntero a arreglo de strings.
 Devuelve la cantidad de directorios encontrados
 o ERROR en caso de algun error.
 Reserva en 'out' cada uno de los directorios registrados en el
 server. El usuario debe encargarse de liberar out.
----------------------------------------------------------------*/
int ListDirs( const string userName,string **out);

/*---------------------------------------------------------------- 
 Devuelve la cantidad de acciones realizadas por los usuarios o 
 ERROR en caso de algun error.
 Reserva en out las 10 últimas acciones de los usuarios, incluido
 el nombre de estos.
 ----------------------------------------------------------------*/
int TopList(string *out);

/*---------------------------------------------------------------- 
 Devuelve la cantidad de acciones realizadas por los usuarios o 
 ERROR en caso de algun error.
 Reserva en out las 10 últimas acciones del usuario userID
 ----------------------------------------------------------------*/
int TopListUser(int userID, string *out);

/*---------------------------------------------------------------- 
 Recibe un fileT y devuelve un arreglo de bytes con el contenido
 binario de ese archivo. El tamaño del arreglo puedo consultarse
 con GetSize(file).
 ----------------------------------------------------------------*/
byte* ReqFile( fileT file );

/*---------------------------------------------------------------- 
 Agrega un directorio existente en el Filesystem del servidor, 
 al repositorio del mismo, quedando registrado en la base de datos
 ----------------------------------------------------------------*/
int DirAdd( string dir  );

/*---------------------------------------------------------------- 
 Borra un archivo del servidor
 ----------------------------------------------------------------*/
int FileRem( fileT file );

/*---------------------------------------------------------------- 
 Agrega un archivo al servidor. Recibe en data el arreglo de bytes
 con el contenido binario del archivo. El usuario es responsable
 de liberar data.
 ----------------------------------------------------------------*/
int FileAdd( fileT file, byte *data );

/*---------------------------------------------------------------- 
 Guarda de manera lineal, los files correspondientes a un
 directorio 'dir', con su respectiva estructura binaria reservado
 en databuffer. El usuario es responsable de liberar files y
 databuffer.
 La función devuelve la cantidad de archivos leidos.
 ----------------------------------------------------------------*/
int ReqDir( string userName, string dir, fileT **files, byte ***databuffer );


#endif