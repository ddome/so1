#include "sqliteADT.h"

/* Restrictions for users */
#define USER_NAME_MAX_LEN 50
#define USER_PASS_MAX_LEN 50
#define USER_MAIL_MAX_LEN 50

typedef struct sqliteCDT
{
    sqlite3 *dbHandle;
    char *dbFile;
    FILE *logFile;
} sqliteCDT;

typedef struct blobBindings
{
    void *data;
    int size;
    int param;
} blobBindings;



/*************************************/
/*	  FUNCOINES ESTATICAS        */
/*************************************/

static void
logError (FILE *errFile, const char *text, ...)
{
    va_list ap;

    if (errFile == NULL || text == NULL)
        return;

    va_start( ap, text );

    vfprintf( errFile, text, ap );
    fprintf( errFile, "\n" );

    va_end( ap );

    return;
}


static long
DBSize(sqliteADT db)
{
    struct stat statbuf;

    if( stat(db->dbFile, &statbuf) == -1)
        return -1;

    return (statbuf.st_size);

}


static int
PrepareSql(sqliteADT db, char *SqlStr, int queryLen,
            sqlite3_stmt **statement, const char **tail)
{
    int rc;
    int n = 0;

    do
    {
        rc = sqlite3_prepare_v2( db->dbHandle, SqlStr, queryLen,
                                statement, tail);

        if( (rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED) )
        {
            n++;
            usleep(SQLTM_TIME);
        }
    }while((n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)));

    if( rc != SQLITE_OK)
    {
        logError(db->logFile, "SqlPrepare-Error-H(%d): (%d) %s \n",
                db->dbHandle, rc, sqlite3_errmsg(db->dbHandle));
    }

    return rc;
}

static int
StepSql(sqliteADT db, sqlite3_stmt *statement)
{
    int rc, n = 0;

    do
    {
        rc = sqlite3_step(statement);

        if( rc == SQLITE_LOCKED )
        {
            /* Note: This will return SQLITE_LOCKED as well... */
            rc = sqlite3_reset(statement);
            n++;
            usleep(SQLTM_TIME);
        }
        else
        {
            if( (rc == SQLITE_BUSY) )
            {
                usleep(SQLTM_TIME);
                n++;
            }
        }
    }while((n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)));

    if( n == SQLTM_COUNT )
    {
        logError(db->logFile, "SqlStep Timeout on handle: %d (rc = %d)\n",
                db->dbHandle, rc);
    }

    if( n > 2 )
    {
        logError(db->logFile, "SqlStep tries on handle %d: %d\n",
                db->dbHandle, n);
    }

    if( rc == SQLITE_MISUSE )
    {
        logError(db->logFile, "sqlite3_step missuse ?? on handle %d\n",
                db->dbHandle);
    }

    return rc;
}

static int
QueryExecute( sqliteADT db, sqlite3_stmt **statement, const char *sql,
                int bindingCount, blobBindings* bindings, int args, ... )
{

    char *query, *tail;
    int queryLen = 0;
    int i;
    va_list ap, apCopy;
    int retCode;

    /* Get the list of arguments */
    va_start( ap, args );
    va_copy( apCopy, ap );

    for ( i = 0; i < args; i++ )
        queryLen += strlen( va_arg( ap, char* ) );

    va_end( ap );

    /* Add the query length to args', and substract 2 for each arg (%s) + NULL */
    queryLen += strlen( sql ) - 2 * args + 1;

    /* Reserve space for query */
    if ( ( query = (char*) calloc( queryLen, sizeof( char ) ) ) == NULL )
    {
        logError( db->logFile, "No hay memoria suficiente en QueryExecute." );
        return DB_NO_MEMORY;
    }

    /* Build query according to received data */
    if ( vsprintf( query, sql, apCopy ) != queryLen - 1 )
    {
        va_end( apCopy );
        free( query );
        logError( db->logFile, "El largo de la consulta no coincide con el valor esperado." );
        return DB_INVALID_ARG;
    }

    va_end( apCopy );

    /* Prepare for execution and retrieve values */
    PrepareSql(db, query, queryLen, statement, (const char**)&tail);


    /* If there are any bindings, do them! */
    if ( bindings != NULL && bindingCount > 0 )
    {
        for ( i = 0; i < bindingCount; i++ )
        {
            sqlite3_bind_blob( *statement, bindings[i].param,
                            bindings[i].data, bindings[i].size,
                                SQLITE_TRANSIENT );
        }
    }

    /* retCode = sqlite3_step( *statement );*/
    retCode = StepSql(db, *statement);

    /* If an error occured, log it */
    if ( retCode != SQLITE_DONE && retCode != SQLITE_ROW )
            logError( db->logFile, "Error al ejecutar la consulta: \"%s\""
                    " - El mensaje de error es: %s", query,
                    sqlite3_errmsg( db->dbHandle ) );

    free( query );
    return retCode;
}



static char *
EscapeString( sqliteADT db, const char *str )
{

    char *outstr;
    int i, j = 0, need = 0;

    for ( i = 0; i < strlen( str ); i++ )
    {
            if ( str[i] == '\'' || str[i] == '%' )
                    need++;
    }

    if ((outstr = (char *)calloc(sizeof(char), strlen(str) + need + 1)) == NULL)
    {
            logError( db->logFile, "No hay memoria suficiente en EscapeString." );
            return NULL;
    }

    for ( i = 0; i < strlen( str ); i++ )
    {
            /* Prevent both, injection in the SQL, and
                     corruption when using sprintf */
            if ( str[i] == '\'' || str[i] == '%' )
                    outstr[j++] = str[i];

            outstr[j++] = str[i];
    }

    return outstr;
}



/*************************************/
/*	  FUNCIONES PUBLICAS         */
/*************************************/


DB_STAT
NewSqliteADT(sqliteADT * db, const char * dbPath, FILE * errorFile)
{
    int ret;
    
    if( db==NULL || dbPath==NULL || errorFile==NULL )
	return DB_INVALID_ARG;
    
    if( ( *db = malloc(sizeof(sqliteCDT)) ) == NULL )
	return DB_NO_MEMORY;
    
    ( *db )->logFile = errorFile;
    ( *db )->dbFile = CreateString((char*)dbPath);

    /*Abre el archivo que contiene la base de datos*/
    ret = sqlite3_open( dbPath, &( ( *db )->dbHandle ) );
    
    if ( ret )
    {
        logError( (*db)->logFile, "Error al intentar abrir la base de datos: %s", sqlite3_errmsg( ( *db )->dbHandle ) );
        return DB_INTERNAL_ERROR;
    }

    return DB_SUCCESS;
}

void
FreeSqliteADT(sqliteADT db)
{
    if( db!=NULL )
    {
	free(db->dbFile);
	fclose(db->logFile);
	sqlite3_close(db->dbHandle);
	free(db);
    }
}


DB_STAT
BuildDatabase( sqliteADT db, const char * schemaPath )
{
    char sqlStr[1024] = {0};
    char line[MAX_LINE];
    FILE *fp = NULL;
    int ret;

    if ( db == NULL || schemaPath == NULL)
            return DB_INVALID_ARG;

    if (DBSize(db) > 0)
        return DB_ALREADY_EXISTS;

    if ( (fp = fopen(schemaPath, "r")) == NULL)
        return DB_INVALID_ARG;

    /*Read the schema*/
    while (fgets(line, MAX_LINE, fp) != NULL)
    {
        strcat(sqlStr, line);

        /* execute if ; found*/
        if ( (strchr(line,';')) != NULL )
        {
            ret = sqlite3_exec( db->dbHandle, sqlStr, NULL, NULL, NULL );

            if ( ret != SQLITE_OK )
            {
                logError( db->logFile, "Error en BuildDatabase - No se puedo crear la tabla: %s",
                        sqlite3_errmsg( db->dbHandle ) );

                return DB_INTERNAL_ERROR;
            }
            sqlStr[0] = '\0';
        }

    }
    fclose(fp);
    return DB_SUCCESS;
}

DB_STAT
AddUser(sqliteADT db,int pid)
{
    sqlite3_stmt *statement;
    int ret;
    char * pidI;
    char * idAux;
    char *sqlSelect = "INSERT INTO users VALUES (NULL, NULL,0,'%s')";

    if (db == NULL || pid == 0)
        return DB_INVALID_ARG;

    if( (idAux=calloc(50,sizeof(char)))==NULL )
    {
	logError( db->logFile, "No hay memoria suficiente en GetUserWithID." );
	return DB_NO_MEMORY;
    }
    sprintf(idAux,"%d",pid);

    if ( ( pidI = EscapeString( db, idAux ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1,pidI);

    free(pidI);
    free(idAux);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        case SQLITE_CONSTRAINT:
            sqlite3_finalize( statement );
            return DB_ALREADY_EXISTS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
    
}

int
AddNameByPid(sqliteADT db,int pid,const char * userName)
{
        sqlite3_stmt *statement;
    int ret;
    char *userN;
    char * pidI;
    char * idAux;
    char *sqlSelect = "UPDATE users SET online = 1,user='%s' WHERE pid ='%s'";

    if (db == NULL || userName == NULL || pid==0 )
        return DB_INVALID_ARG;

    if( (idAux=calloc(50,sizeof(char)))==NULL )
    {
	logError( db->logFile, "No hay memoria suficiente en GetUserWithID." );
	return DB_NO_MEMORY;
    }
    sprintf(idAux,"%d",pid);

    if ( ( pidI = EscapeString( db, idAux ) ) == NULL )
        return DB_NO_MEMORY;
    free(idAux);

    if ( ( userN = EscapeString( db, userName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 2, userN,pidI);

    free(pidI);
    free(userN);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        case SQLITE_CONSTRAINT:
            sqlite3_finalize( statement );
            return DB_ALREADY_EXISTS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
UserPidExist(sqliteADT db, int pid,int * boolRet)
{
    sqlite3_stmt *statement;
    int ret;
    char * idAux;
    char *pidI;
    char *sqlSelect = "SELECT pid FROM users WHERE pid='%s'";

    if (db == NULL || pid==0)
        return DB_INVALID_ARG;

    if( (idAux=calloc(50,sizeof(char)))==NULL )
    {
	logError( db->logFile, "No hay memoria suficiente en UserPidExist." );
	return DB_NO_MEMORY;
    }
    sprintf(idAux,"%d",pid);

    if ( ( pidI = EscapeString( db, idAux ) ) == NULL )
        return DB_NO_MEMORY;

    free(idAux);
    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1, pidI);
    *boolRet=sqlite3_column_int(statement,0);

    free(pidI);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        case SQLITE_CONSTRAINT:
            sqlite3_finalize( statement );
            return DB_ALREADY_EXISTS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}


DB_STAT
IsUserOnline(sqliteADT db, const char * userName,int * boolRet)
{
    sqlite3_stmt *statement;
    int ret;
    char *userN;
    char *sqlSelect = "SELECT online FROM users WHERE user='%s'";

    if (db == NULL || userName == NULL)
        return DB_INVALID_ARG;

    if ( ( userN = EscapeString( db, userName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1, userN);
    *boolRet=sqlite3_column_int(statement,0);
    free(userN);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        case SQLITE_CONSTRAINT:
            sqlite3_finalize( statement );
            return DB_ALREADY_EXISTS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
IsUserOnlinePID(sqliteADT db, int pid,int * boolRet)
{
    sqlite3_stmt *statement;
    int ret;
    char * pidI;
    char * idAux;
    char *sqlSelect = "SELECT online FROM users WHERE pid='%s'";

    if (db == NULL || pid==0 )
        return DB_INVALID_ARG;

    if( (idAux=calloc(50,sizeof(char)))==NULL )
    {
	logError( db->logFile, "No hay memoria suficiente en IsUserOnlinePID." );
	return DB_NO_MEMORY;
    }
    sprintf(idAux,"%d",pid);

    if ( ( pidI = EscapeString( db, idAux ) ) == NULL )
        return DB_NO_MEMORY;
    free(idAux);

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1, pidI);
    *boolRet=sqlite3_column_int(statement,0);
    free(pidI);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        case SQLITE_CONSTRAINT:
            sqlite3_finalize( statement );
            return DB_ALREADY_EXISTS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
UnlinkAllDirsToUser(sqliteADT db,const char * userName)
{
    sqlite3_stmt *statement;
    int ret;
    char *userN;
    char *sqlSelect =   "DELETE FROM users_dir WHERE user_id = (SELECT id FROM users WHERE user='%s')";

    if (db == NULL || userName == NULL)
        return DB_INVALID_ARG;

    if ( ( userN = EscapeString( db, userName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1, userN);

    free(userN);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}


DB_STAT
UserOffline(sqliteADT db,const char * userName)
{
    sqlite3_stmt *statement;
    int ret;
    char *userN;
    char *sqlSelect =   "DELETE FROM users WHERE user ='%s'";

    if (db == NULL || userName == NULL)
        return DB_INVALID_ARG;

    if ( ( userN = EscapeString( db, userName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1, userN);

    free(userN);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
UnlinkAllDirs(sqliteADT db)
{
    sqlite3_stmt *statement;
    int ret;
    char *sqlSelect =   "DELETE FROM users_dir";

    if (db == NULL)
        return DB_INVALID_ARG;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 0);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
AllOffline(sqliteADT db)
{
    sqlite3_stmt *statement;
    int ret;
    char *sqlSelect = "DELETE FROM users";

    if (db == NULL )
        return DB_INVALID_ARG;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 0);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
ShowOnline(sqliteADT db,pqADT queue)
{
    sqlite3_stmt *statement;
    int ret;
    char userName[MAX_USR_NAME] = {0};
    char *sqlSelect = "SELECT user FROM users WHERE online=1";

    if ( db == NULL || queue == NULL )
        return DB_INVALID_ARG;

    ret = QueryExecute( db, &statement, sqlSelect, 0, NULL, 0 );

    while ( ret == SQLITE_ROW )
    {
        strncpy(userName, (char *) sqlite3_column_text(statement, 0), MAX_USR_NAME);
        userName[MAX_USR_NAME-1] = 0;

        if ((Enqueue(queue, &userName,1)) == 1)
            ret = sqlite3_step( statement );
        else
        {
            sqlite3_finalize( statement );
            return DB_INTERNAL_ERROR;
        }
    }

    sqlite3_finalize( statement );

    if ( ret != SQLITE_DONE )
        return DB_INTERNAL_ERROR;

    return DB_SUCCESS;
}

DB_STAT
ShowOnlineByPID(sqliteADT db,pqADT queue)
{
    sqlite3_stmt *statement;
    int ret,aux;
    char *sqlSelect = "SELECT pid FROM users WHERE online=1";

    if ( db == NULL || queue == NULL )
        return DB_INVALID_ARG;

    ret = QueryExecute( db, &statement, sqlSelect, 0, NULL, 0 );

    while ( ret == SQLITE_ROW )
    {
        aux=sqlite3_column_int(statement,0);

        if ((Enqueue(queue, (void *)aux,1)) == 1)
            ret = sqlite3_step( statement );
        else
        {
            sqlite3_finalize( statement );
            return DB_INTERNAL_ERROR;
        }
    }

    sqlite3_finalize( statement );

    if ( ret != SQLITE_DONE )
        return DB_INTERNAL_ERROR;

    return DB_SUCCESS;
}

DB_STAT
PIDToUserName(sqliteADT db,int pid,pqADT queue)
{
    sqlite3_stmt *statement;
    int ret;
    char * idAux;
    char * pidI;
    char auxName[MAX_USR_NAME]={0};
    char *sqlSelect = "SELECT user FROM users WHERE pid='%s'";
   

    if ( db == NULL || queue == NULL )
        return DB_INVALID_ARG;
    
    if( (idAux=calloc(50,sizeof(char)))==NULL )
    {
	logError( db->logFile, "No hay memoria suficiente en GetUserWithID." );
	return DB_NO_MEMORY;
    }
    sprintf(idAux,"%d",pid);

    if ( ( pidI = EscapeString( db, idAux ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1,pidI);

    free(pidI);
    free(idAux);
    while ( ret == SQLITE_ROW )
    {
        strncpy(auxName, (char *) sqlite3_column_text(statement, 0), MAX_USR_NAME);
        auxName[MAX_USR_NAME-1] = 0;

        if ((Enqueue(queue, &auxName,1)) == 1)
            ret = sqlite3_step( statement );
        else
        {
            sqlite3_finalize( statement );
            return DB_INTERNAL_ERROR;
        }
    }

    sqlite3_finalize( statement );

    if ( ret != SQLITE_DONE )
    {
        return DB_INTERNAL_ERROR;
    }

    return DB_SUCCESS;
}


DB_STAT
RegisterDir(sqliteADT db,const char * pathName)
{
    sqlite3_stmt *statement;
    int ret;
    char *dirP;
    char *sqlSelect = "INSERT INTO dirs VALUES (NULL, '%s')";

    if (db == NULL || pathName == NULL)
        return DB_INVALID_ARG;

    if ( ( dirP = EscapeString( db, pathName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1, dirP);

    free(dirP);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        case SQLITE_CONSTRAINT:
            sqlite3_finalize( statement );
            return DB_ALREADY_EXISTS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
LinkDirToUser(sqliteADT db,const char * pathName,const char * userName)
{
    sqlite3_stmt *statement;
    int ret;
    char * dirP;
    char * userN;
    char *sqlSelect = "INSERT INTO users_dir VALUES ( (SELECT id FROM users WHERE user = '%s') , (SELECT id FROM dirs WHERE dirname = '%s') )";
    
    if (db == NULL || pathName == NULL || userName == NULL)
        return DB_INVALID_ARG;

    if ( ( userN = EscapeString( db, userName ) ) == NULL )
        return DB_NO_MEMORY;

    if ( ( dirP = EscapeString( db, pathName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 2, userN, dirP);

    free(userN);
    free(dirP);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        case SQLITE_CONSTRAINT:
            sqlite3_finalize( statement );
            return DB_ALREADY_EXISTS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
DeleteDir(sqliteADT db ,const char * pathName)
{
    sqlite3_stmt *statement;
    int ret;
    char *dirP;
    char *sqlSelect =   "DELETE FROM dirs WHERE dirname = '%s'";

    if (db == NULL || pathName == NULL)
        return DB_INVALID_ARG;

    if ( ( dirP = EscapeString( db, pathName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1, dirP);

    free(dirP);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
UnlinkUserToDir(sqliteADT db,const char * pathName,const char * userName)
{
    sqlite3_stmt *statement;
    int ret;
    char *userN;
    char *sqlSelect =   "DELETE FROM users_dir WHERE user_id = (SELECT id FROM users WHERE user = '%s') AND dir_id=(SELECT id FROM dirs WHERE dirname = '%s')";

    if (db == NULL || userName == NULL)
        return DB_INVALID_ARG;

    if ( ( userN = EscapeString( db, userName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 1, userN);

    free(userN);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}

DB_STAT
ListAllDirs(sqliteADT db,pqADT queue)
{
    sqlite3_stmt *statement;
    int ret;
    char pathName[MAX_DIR_NAME] = {0};
    char *sqlSelect = "SELECT dirname FROM dirs";

    if ( db == NULL || queue == NULL )
        return DB_INVALID_ARG;

    ret = QueryExecute( db, &statement, sqlSelect, 0, NULL, 0 );

    while ( ret == SQLITE_ROW )
    {
        strncpy(pathName, (char *) sqlite3_column_text(statement, 0), MAX_DIR_NAME);
        pathName[MAX_DIR_NAME-1] = 0;

        if ((Enqueue(queue, &pathName,1)) == 1)
            ret = sqlite3_step( statement );
        else
        {
            sqlite3_finalize( statement );
            return DB_INTERNAL_ERROR;
        }
    }

    sqlite3_finalize( statement );

    if ( ret != SQLITE_DONE )
        return DB_INTERNAL_ERROR;

    return DB_SUCCESS;
}



DB_STAT
ListUsersLinkToDir(sqliteADT db,const char * pathName,pqADT queue)
{
    sqlite3_stmt *statement;
    int ret;
    char * dirP;
    char auxPath[MAX_DIR_NAME]={0};
    char *sqlSelect = "SELECT users.user FROM users INNER JOIN users_dir ON users_dir.user_id = users.id INNER JOIN dirs ON users_dir.dir_id = dirs.id WHERE users.online = 1 AND dirs.dirname = '%s'";
   

    if ( db == NULL || queue == NULL )
        return DB_INVALID_ARG;
    
    if ( ( dirP = EscapeString( db, pathName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute( db, &statement, sqlSelect, 0, NULL, 1,dirP );

    free(dirP);
    while ( ret == SQLITE_ROW )
    {
        strncpy(auxPath, (char *) sqlite3_column_text(statement, 0), MAX_DIR_NAME);
        auxPath[MAX_DIR_NAME-1] = 0;

        if ((Enqueue(queue, &auxPath,1)) == 1)
            ret = sqlite3_step( statement );
        else
        {
            sqlite3_finalize( statement );
            return DB_INTERNAL_ERROR;
        }
    }

    sqlite3_finalize( statement );

    if ( ret != SQLITE_DONE )
    {
        return DB_INTERNAL_ERROR;
    }

    return DB_SUCCESS;
}

DB_STAT
    UsersLinkToDirCant(sqliteADT db,const char * pathName,pqADT queue, int * resp)
{
  sqlite3_stmt *statement;
  int ret;
  char * dirP;
  char auxPath[MAX_DIR_NAME]={0};
  char *sqlSelect = "SELECT count(users.id) FROM users INNER JOIN users_dir ON users_dir.user_id = users.id INNER JOIN dirs ON users_dir.dir_id = dirs.id WHERE users.online = 1 AND dirs.dirname = '%s'";
   

  if ( db == NULL || queue == NULL )
    return DB_INVALID_ARG;
    
  if ( ( dirP = EscapeString( db, pathName ) ) == NULL )
    return DB_NO_MEMORY;

  ret = QueryExecute( db, &statement, sqlSelect, 0, NULL, 1,dirP );

  free(dirP);
  while ( ret == SQLITE_ROW )
  {
    *resp=sqlite3_column_int(statement, 0);

  /*  if ((Enqueue(queue, (void *)aux,1)) == 1)
      ret = sqlite3_step( statement );
    else
    {
      sqlite3_finalize( statement );
      return DB_INTERNAL_ERROR;
    }*/
  }

  sqlite3_finalize( statement );

  if ( ret != SQLITE_DONE )
  {
    return DB_INTERNAL_ERROR;
  }

  return DB_SUCCESS;
}

DB_STAT
ListPIDsLinkToDir(sqliteADT db,const char * pathName,pqADT queue)
{
    sqlite3_stmt *statement;
    int ret,aux;
    char * dirP;
    char *sqlSelect = "SELECT users.pid FROM users INNER JOIN users_dir ON users_dir.user_id = users.id INNER JOIN dirs ON users_dir.dir_id = dirs.id WHERE users.online = 1 AND dirs.dirname = '%s'";
   

    if ( db == NULL || queue == NULL )
        return DB_INVALID_ARG;
    
    if ( ( dirP = EscapeString( db, pathName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute( db, &statement, sqlSelect, 0, NULL, 1,dirP );

    free(dirP);
    while ( ret == SQLITE_ROW )
    {
        aux=sqlite3_column_int(statement, 0);

        if ((Enqueue(queue, (void *)aux,1)) == 1)
            ret = sqlite3_step( statement );
        else
        {
            sqlite3_finalize( statement );
            return DB_INTERNAL_ERROR;
        }
    }

    sqlite3_finalize( statement );

    if ( ret != SQLITE_DONE )
    {
        return DB_INTERNAL_ERROR;
    }

    return DB_SUCCESS;
}


DB_STAT
ListDirsLinkToUser(sqliteADT db,const char * userName,pqADT queue)
{
    sqlite3_stmt *statement;
    int ret;
    char * userN;
    char auxName[MAX_USR_NAME]={0};
    char *sqlSelect = "SELECT dirs.dirname FROM dirs INNER JOIN users_dir ON users_dir.dir_id = dirs.id INNER JOIN users ON users.id = users_dir.user_id WHERE users.user = '%s'";
   

    if ( db == NULL || queue == NULL )
        return DB_INVALID_ARG;
    
    if ( ( userN = EscapeString( db, userName ) ) == NULL )
        return DB_NO_MEMORY;

    ret = QueryExecute( db, &statement, sqlSelect, 0, NULL, 1,userN );

    free(userN);
    while ( ret == SQLITE_ROW )
    {
        strncpy(auxName, (char *) sqlite3_column_text(statement, 0), MAX_USR_NAME);
        auxName[MAX_USR_NAME-1] = 0;

        if ((Enqueue(queue, &auxName,1)) == 1)
            ret = sqlite3_step( statement );
        else
        {
            sqlite3_finalize( statement );
            return DB_INTERNAL_ERROR;
        }
    }

    sqlite3_finalize( statement );

    if ( ret != SQLITE_DONE )
    {
        return DB_INTERNAL_ERROR;
    }

    return DB_SUCCESS;
}

DB_STAT
GetLast10(sqliteADT db, pqADT queue)
{
    sqlite3_stmt *statement;
    int ret;
    char logName[MAX_LOG_LEN] = {0};
    char *sqlSelect =   "SELECT timestamp || ' ' || 'Usuario: ' || user || ' ' || 'Accion: ' || action || ' '||'Archivo: ' || fileP FROM logs LIMIT 10;";

    if ( db == NULL || queue == NULL )
        return DB_INVALID_ARG;


    ret = QueryExecute( db, &statement, sqlSelect, 0, NULL, 0);

    while ( ret == SQLITE_ROW )
    {
        strncpy(logName, (char *) sqlite3_column_text(statement, 0), MAX_LOG_LEN);
        logName[MAX_LOG_LEN-1] = 0;

        if ((Enqueue(queue, &logName,1)) == 1)
            ret = sqlite3_step( statement );
        else
        {
            sqlite3_finalize( statement );
            return DB_INTERNAL_ERROR;
        }
    }

    sqlite3_finalize( statement );

    if ( ret != SQLITE_DONE )
        return DB_INTERNAL_ERROR;

    return DB_SUCCESS;
}


DB_STAT
GetLast10User(sqliteADT db, pqADT queue,const char *userName)
{
    sqlite3_stmt *statement;
    int ret;
    char logName[MAX_LOG_LEN] = {0};
    char *userE = NULL;
    char *sqlSelect = "SELECT timestamp || ' ' || 'Usuario: ' || user || ' ' || 'Accion: ' || action || ' '||'Archivo: ' || fileP FROM logs WHERE user = '%s' LIMIT 10;";

    if ( db == NULL || queue == NULL || userName == NULL)
        return DB_INVALID_ARG;

    if ( ( userE = EscapeString( db, userName ) ) == NULL )
                return DB_NO_MEMORY;

    ret = QueryExecute( db, &statement, sqlSelect, 0, NULL, 1, userE );
    free(userE);

    while ( ret == SQLITE_ROW )
    {
        strncpy(logName, (char *) sqlite3_column_text(statement, 0), MAX_LOG_LEN);
        logName[MAX_LOG_LEN-1] = 0;

        if ((Enqueue(queue, &logName,1)) == 1)
            ret = sqlite3_step( statement );
        else
        {
            sqlite3_finalize( statement );
            return DB_INTERNAL_ERROR;
        }
    }

    sqlite3_finalize( statement );

    if ( ret != SQLITE_DONE )
        return DB_INTERNAL_ERROR;

    return DB_SUCCESS;
}



DB_STAT
AddLog(sqliteADT db, const char *userName,const char * fileName ,const char *action)
{
    sqlite3_stmt *statement;
    int ret;
    char *userN, *actionN,*fileN;
    char *sqlSelect = "INSERT INTO logs VALUES ((select user from users where "
                      "user = '%s'), '%s','%s' ,DATETIME('NOW'));";

    if (db == NULL || userName == NULL || action == NULL || fileName==NULL)
        return DB_INVALID_ARG;

    /*TODO: In the next lines inside the if it should free
            malloced strings in this function */
    if ( ( userN = EscapeString( db, userName ) ) == NULL )
        return DB_NO_MEMORY;

    if ( ( actionN = EscapeString( db, action ) ) == NULL )
        return DB_NO_MEMORY;

    if ( ( fileN = EscapeString( db, fileName ) ) == NULL )
        return DB_NO_MEMORY;


    ret = QueryExecute(db, &statement, sqlSelect, 0, NULL, 3, userN, actionN, fileN);

    free(userN);
    free(actionN);
    free(fileN);

    switch (ret)
    {
        case SQLITE_DONE:
            sqlite3_finalize( statement );
            return DB_SUCCESS;

        case SQLITE_CONSTRAINT:
            sqlite3_finalize( statement );
            return DB_ALREADY_EXISTS;

        default:
            sqlite3_finalize(statement);
            return DB_INTERNAL_ERROR;
    }
}





























































































































