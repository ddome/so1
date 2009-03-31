#ifndef __SQLITE_ADT__
#define __SQLITE_ADT__

/*---------------------------------------------------------------------------*/

/*		CONTRATO DEL TAD PARA EL MANEJO DE LA BASE DE DATOS
 * Este TAD probee al usuario con funciones para manejar informacion almacenada
 *en una base de datos. El motor de base de datos utilizado en este TAD es el
 *SQLite. El codigo necesario para compilar este TAD se encuentra en la carpeta
 *SQLite.
*/


/*
 * Includes
*/
#include <stdio.h>
#include "../Lib/pqADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../SQLite/sqlite3.h"
#include "../Lib/defines.h"
#include "../Lib/pqADT.h"
#include "../Lib/genlib.h"

/** Timeout values on busy or lock conditions   **/
/** if you observe lock errors you might try to **/
/** increase the values.     **/
/** SQLTM_COUNT*SQLTM_TIME micro seg timeout **/
#define SQLTM_COUNT       500
#define SQLTM_TIME        1000



/* Este es el tipo abstracto usado para representar una instancia particular
 *del TAD para le manejo de de la base de datos.
*/
typedef struct sqliteCDT * sqliteADT;



/* 
 *
*/
typedef enum { DB_SUCCESS = 0, DB_INVALID_ARG, DB_NO_MATCH, DB_NO_MEMORY,
            DB_INTERNAL_ERROR, DB_ACCESS_DENIED, DB_ALREADY_EXISTS } DB_STAT;





DB_STAT NewSqliteADT(sqliteADT * db, const char * dbPath, FILE * errorFile);

DB_STAT BuildDatabase( sqliteADT db, const char * schemaPath );

void FreeSqliteADT(sqliteADT db);



/*por default el user esta offline*/
DB_STAT AddUser(sqliteADT db, const char * userName,int pid);/*TESTEADA*/
/*Online en 1*/
DB_STAT UserOnline(sqliteADT db,const char * userName);/*TESTEADA*/
/*online en 0 es offline*/
DB_STAT UserOffline(sqliteADT db,const char * userName);/*TESTEADA*/

DB_STAT IsUserOnline(sqliteADT db, const char * userName,int * boolRet);

DB_STAT AllOffline(sqliteADT db);/*TESTEADA*/

DB_STAT ShowOnline(sqliteADT db,pqADT queue);/*TESTEADA*/

DB_STAT ShowOnlineByID(sqliteADT db,pqADT queue);/*TESTEADA*/

DB_STAT DeleteUser(sqliteADT db ,const char * userName);

DB_STAT GetUserID(sqliteADT db ,const char * userName,int * ID);

DB_STAT GetUserWithID(sqliteADT db,int ID, char ** userName);




DB_STAT RegisterDir(sqliteADT db,const char * pathName);/*TESTEADA*/

DB_STAT LinkDirToUser(sqliteADT db,const char * pathName,const char * userName);/*TESTEADA*/

DB_STAT DeleteDir(sqliteADT db,const char * pathName);/*TESTEADA*/

DB_STAT UnlinkUserToDir(sqliteADT db,const char * pathName ,const char * userName);/*TESTEADA*/

DB_STAT ListAllDirs(sqliteADT db,pqADT queue);/*TESTEADA*/
/*Lista los usuarios registrados a un directorio determinado*/
DB_STAT ListUsersLinkToDir(sqliteADT db,const char * pathName,pqADT queue);
/*Lista los directorios registrados a un usuario determinado*/
DB_STAT ListDirsLinkToUser(sqliteADT db,const char * userName,pqADT queue);






DB_STAT GetLast10(sqliteADT db, pqADT queue);

DB_STAT GetLast10User(sqliteADT db, const char *userName,pqADT queue);

DB_STAT LogAction(sqliteADT db, const char * userName, const char *action);



#endif