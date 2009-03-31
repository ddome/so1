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
	(*out)[i]=Dequeue(queue);
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
    if( ret==DB_ALREADY_EXISTS )
    {
	IsUserOnline(db,userName,&boolRet);
	if(boolRet==1)
	    return ERROR;
    }
    return OK;
}

int
SetClientOnline(const char * userName)
{
    UserOnline(db,userName);
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
	RegisterDir(db,pathName);
	return OK;
}

int
RegisterDirToUser(const char * pathName,const char *userName)
{
	LinkDirToUser(db,pathName,userName);
	return OK;
}

int
UnRegisterDirFromUser(const char * pathName,const char * userName)
{
	UnlinkUserToDir(db,pathName,userName);
	return OK;
}

int
NewLogEntry(const char * userName,const char * action)
{
    int boolRet;
    IsUserOnline(db,userName,&boolRet);
    if(boolRet==0)
	return ERROR;
    LogAction(db,userName,action);
    return OK;
}

int
GetTopList(const char * userName,char ***out)
{
    pqADT queue;
    int i=0;
    queue=NewPQ((void*(*)(void*))CopyString,(void*(*)(void*))FreeString);
    if(queue==NULL)
    {
	fprintf(stderr,"Error al crear la cola en TopList.\n");
	return ERROR;
    }
    if(userName==NULL)
	GetLast10(db,queue);
    else
	GetLast10User(db,userName,queue);
    
    if( (*out=calloc(QueueDepth(queue)+1,sizeof(char*)))==NULL )
    {
	fprintf(stderr,"Error al alocar espacio en GetListDirs.\n");
	return ERROR;
    }
    while(!PQIsEmpty(queue))
    {
	(*out)[i]=Dequeue(queue);
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
	(*out)[i]=(char*)Dequeue(queue);
	i++;
    }
    FreePQ(&queue);
    
    return OK;
}











