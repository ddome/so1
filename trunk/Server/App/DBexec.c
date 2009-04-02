#include "DBexec.h"

static sqliteADT db = NULL;

char *
FreeString(char * string)
{
    if(string==NULL)
	return NULL;
    
    free(string);
    return NULL;
}

/*Esta funcion iniciaria la base de datos.*/
int
InitBD(void)
{
    sqliteADT dbAux;
    FILE * errorFile;
    char * dbPath="./database.sql";
    char * errorFilePath="./DB_ERR";
    char * schemaPath="./schema.sql";
    int ret;
    if( (errorFile=fopen(errorFilePath,"w"))==NULL )
    {
	    fprintf(stderr,"El archivo de errores de la base de datos no pedo ser abierto, se procedera sin registrar posibles errores.\n");
	    return ERROR;
    }
    
    if( NewSqliteADT(&dbAux,dbPath,errorFile)!=DB_SUCCESS )
    {
	    fprintf(stderr,"Error fatal al intentar abrir la base de datos. No se puede continuar.\n");
	    return ERROR;
    }
    ret=BuildDatabase(dbAux,schemaPath);
    if( ret!=DB_ALREADY_EXISTS && ret!=DB_SUCCESS)
    {
	    fprintf(stderr,"Error fatal al intentar abrir la base de datos. No se puede continuar.\n");
	    return ERROR;
    }
    if (db==NULL)
	db=dbAux;
    
    return OK;
}
/*Listar directorios ligados a un usuario en particular. cambie el prototipo de la funcion.*/
int
GetListDirs(const char * userName, string **out)
{
    pqADT queue;
    int i=0;
    char * aux;
    queue=NewPQ((void*(*)(void*))CopyString,(void*(*)(void*))FreeString);
    if(queue==NULL)
    {
	fprintf(stderr,"Error al crear la cola en TopList.\n");
	return ERROR;
    }

    ListDirsLinkToUser(db,userName,queue);

    if( (*out=calloc(QueueDepth(queue)+1,sizeof(char*)))==NULL )
    {
	fprintf(stderr,"Error al alocar espacio en GetListDirs.\n");
	return ERROR;
    }
    while(!PQIsEmpty(queue))
    {
	aux=Dequeue(queue);
	(*out)[i]=CreateString(aux);
	free(aux);
	i++;
    }
    FreePQ(&queue);
    return OK;
}

int
AddClient(const char * userName, int pid)
{
    int ret;
    int boolRet;
    ret=AddUser(db, userName,pid);
    if( ret==DB_ALREADY_EXISTS || ret==DB_SUCCESS )
    {
	IsUserOnline(db,userName,&boolRet);
	if(boolRet==1)
	    return ERROR;
    }
    if(ret==DB_INTERNAL_ERROR)
	return DB_ERROR;
    return OK;
}

int
SetClientOnline(const char * userName)
{
    /*El usuario tiene q exitir*/
    UserOnline(db,userName);
    return OK;
}

int
SetClientOffline(const char * userName)
{
    /*El usuario tiene q exitir y este online*/
    UserOffline(db,userName);
    return OK;
}

int
SetAllClientsOffline(void)
{
    AllOffline(db);
    return OK;
}

int
GetUserStatus(const char * nameName)
{
    int boolRet;
    IsUserOnline(db,nameName,&boolRet);
    return boolRet;
}

int
NewDir(const char * pathName)
{
    int ret=RegisterDir(db,pathName);
    if(ret==DB_INTERNAL_ERROR)
	return DB_ERROR;
    return OK;
}

int
RegisterDirToUser(const char * pathName,const char *userName)
{
    int ret=LinkDirToUser(db,pathName,userName);
    if(ret==DB_INTERNAL_ERROR)
	return DB_ERROR;
    else if(ret==DB_ALREADY_EXISTS)
	return ERROR;
    else
	return OK;
}

int
UnRegisterDirFromUser(const char * pathName,const char * userName)
{
    /*checkear postcondiciones*/
    UnlinkUserToDir(db,pathName,userName);
    return OK;
}

int
NewLogEntry(const char * userName,const char * fileName,const char * action)
{
    int boolRet,ret;
    IsUserOnline(db,userName,&boolRet);
    if(boolRet==0)
	return ERROR;
    ret=AddLog(db,userName,fileName ,action);
    if(ret!=DB_SUCCESS)
	return ERROR;
    return OK;
}

int
GetTopList(const char * userName,char ***out)
{
    pqADT queue;
    int i=0;
    char * aux;
    queue=NewPQ((void*(*)(void*))CopyString,(void*(*)(void*))FreeString);
    if(queue==NULL)
    {
	fprintf(stderr,"Error al crear la cola en TopList.\n");
	return ERROR;
    }
    if(userName==NULL)
	GetLast10(db,queue);
    else
	GetLast10User(db,queue,userName);
    
    if( (*out=calloc(QueueDepth(queue)+1,sizeof(char*)))==NULL )
    {
	fprintf(stderr,"Error al alocar espacio en GetListDirs.\n");
	return ERROR;
    }
    while(!PQIsEmpty(queue))
    {
	aux=Dequeue(queue);
	(*out)[i]=CreateString(aux);
	free(aux);
	i++;
    }
    FreePQ(&queue);
    
    return OK;
}

int
GetUserOnlineList( char *** out )
{
    pqADT queue;
    int i=0;
    char * aux;
    queue=NewPQ((void*(*)(void*))CopyString,(void*(*)(void*))FreeString);
    if(queue==NULL)
    {
	fprintf(stderr,"Error al crear la cola en TopList.\n");
	return ERROR;
    }
    
    ShowOnline(db,queue);
    
    if( ( (*out)=calloc(QueueDepth(queue)+1,sizeof(char*)) )==NULL )
    {
	fprintf(stderr,"Error al alocar espacio en GetListDirs.\n");
	return ERROR;
    }
    while(!PQIsEmpty(queue))
    {
	aux=Dequeue(queue);
	(*out)[i]=CreateString(aux);
	free(aux);
	i++;
    }
    FreePQ(&queue);
    
    return OK;
}

int
FreeDatabase(void)
{
    FreeSqliteADT(db);
    return OK;
}











