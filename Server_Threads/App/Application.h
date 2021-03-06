#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "../Lib/genlib.h"
#include "../Lib/defines.h"
#include "fileHandler.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_PATH "backup/"

#include "../sqliteADT/sqliteADT.h"
#include "../Lib/pqADT.h"
#include "DBexec.h"
#include "../Lib/Error.h"

#define NEW_USR_EXIST 0
#define NEW_USR_OK    1
#define NEW_USR_ERROR 2

#define NEW_USRNAME_EXIST 3
#define NEW_USRNAME_OK    4
#define NEW_USRNAME_ERROR 5


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
int NewClient(int pid);

int ConnectUser(int pid,char * userName);

/*---------------------------------------------------------------- 
 Recibe un nombre de usuario y un puntero a arreglo de strings.
 Devuelve la cantidad de directorios encontrados
 o ERROR en caso de algun error.
 Reserva en 'out' cada uno de los directorios registrados en el
 server. El usuario debe encargarse de liberar out.
----------------------------------------------------------------*/
int ListDirs( const string userName);

/*---------------------------------------------------------------- 
 Lista los usuarios registrados en el servidor.
 Devuelve la cantidad de usuarios en la lista.
 ----------------------------------------------------------------*/
int UserList( void );

/*---------------------------------------------------------------
 Almacena que tipo de accion realizo un usuario sobre un archivo
 determinado. Retorna ERROR en caso de algun error. OK en caso
 contrario.
---------------------------------------------------------------*/
int LogAction(const char * userName,const char * fileName,const char * action);

/*---------------------------------------------------------------- 
 Devuelve la cantidad de acciones realizadas por los usuarios o 
 ERROR en caso de algun error.
 Reserva en out las 10 últimas acciones de los usuarios, incluido
 el nombre de estos.
 ----------------------------------------------------------------*/
int TopList(void);

/*---------------------------------------------------------------- 
 Devuelve la cantidad de acciones realizadas por los usuarios o 
 ERROR en caso de algun error.
 Reserva en out las 10 últimas acciones del usuario userID
 ----------------------------------------------------------------*/
int TopListUser(string userName);

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

/*---------------------------------------------------------------- 
 Borra un directorio de la base de directorios actualizables de un
 cliente
 ----------------------------------------------------------------*/
int DelDir( string userName, string dir );

/*---------------------------------------------------------------- 
 Remueve al un usuario de la lista de usuarios conectados.
 ----------------------------------------------------------------*/
int DisconnectUser(string userName);

/*---------------------------------------------------------------- 
 Remueve a todos los usuarios de la lista de usuarios conectados.
 ----------------------------------------------------------------*/
int DisconnectAllUsers(void);

/*---------------------------------------------------------------- 
 Termina con la aplicacion. Libera memoria y cierra archivos en uso
 ----------------------------------------------------------------*/
int CloseApplication(void);

/*----------------------------------------------------------------
 Guarda en out la lista de todos los directorios sincronizables.
 retorna la cantidad de directorios.
----------------------------------------------------------------*/
int ListAllSyncDirs( string **out);

/*----------------------------------------------------------------
 Devuelve la cantidad de usuarios sincronizados a un directorio
 determinado.
----------------------------------------------------------------*/
int CantUsersLinkToDir(char * pathName);


/*----------------------------------------------------------------
 Retorna en un vector de enteros los pids de los usuarios
 registrados a un directorio particular.
----------------------------------------------------------------*/
int * PIDsLinkToDir(char * pathName);


char * ConvertPIDToUserName(int pid);

int UserAddDir( string userName, string dirName );

#endif


