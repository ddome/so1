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
    if(userName==NULL || out==NULL)
        return ERROR;
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
AddClient(int pid)
{
    int ret;
    if(pid<=0)
        return ERROR;
    ret=AddUser(db,pid);
    if(ret==DB_SUCCESS)
	return OK;
    else if(ret==DB_ALREADY_EXISTS)
	return ERROR;
    else
	return DB_ERROR;
    return OK;
}

int
SetName(int pid,const char * userName)
{
    int ret;
    int boolRet;
    if(pid<=0 || userName==NULL)
        return ERROR;
    UserPidExist(db,pid,&boolRet);
    if(boolRet<=0)
	return ERROR;
    ret=AddNameByPid(db,pid,userName);
    if(ret==DB_SUCCESS)
    {
	return OK;
    }
    else if(ret==DB_ALREADY_EXISTS)
    {
	return ERROR;
    }
    else
	return DB_ERROR;
}


int
SetClientOffline(const char * userName)
{
    if(userName==NULL)
        return ERROR;
    UnlinkAllDirsToUser(db,userName);
    UserOffline(db,userName);
    return OK;
}

int
SetAllClientsOffline(void)
{
    UnlinkAllDirs(db);
    AllOffline(db);
    return OK;
}

int
GetUserStatus(const char * userName)
{
    int boolRet;
    if(userName==NULL)
        return ERROR;
    IsUserOnline(db,userName,&boolRet);
    return boolRet;
}

int
NewDir(const char * pathName)
{
    int ret;
    if(pathName==NULL)
        return ERROR;
    ret=RegisterDir(db,pathName);
    if(ret==DB_INTERNAL_ERROR)
	return DB_ERROR;
    return OK;
}

int
RegisterDirToUser(const char * pathName,const char *userName)
{
    int ret;
    if(pathName==NULL || userName==NULL)
        return ERROR;
    ret=LinkDirToUser(db,pathName,userName);
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


int
GetListDirsAll(char *** out)
{
    pqADT queue;
    int i=0,cant;
    char * aux;
    queue=NewPQ((void*(*)(void*))CopyString,(void*(*)(void*))FreeString);
    if(queue==NULL)
    {
	fprintf(stderr,"Error al crear la cola en TopList.\n");
	return ERROR;
    }

    ListAllDirs(db,queue);
    cant=QueueDepth(queue);
    if( (*out=calloc(cant+1,sizeof(char*)))==NULL )
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
    return cant;
}

int
GetCantUsersLinkToDir(char * pathName)
{
    pqADT queue;
    int cant=0;
    queue=NewPQ((void*(*)(void*))CopyString,(void*(*)(void*))FreeString);
    if(queue==NULL)
    {
        fprintf(stderr,"Error al crear la cola en TopList.\n");
        return ERROR;
    }

    ListUsersLinkToDir(db,pathName,queue);
    cant=QueueDepth(queue);

    FreePQ(&queue);
    return cant;
}

int
GetListPIDsLinkToDir(char * pathName,int ** pids)
{
    pqADT queue;
    int i=0,cant;
    int * aux;
    queue=NewPQ(NULL,(void*(*)(void*))FreeString);
    if(queue==NULL)
    {
        fprintf(stderr,"Error al crear la cola en TopList.\n");
        return ERROR;
    }

    ListPIDsLinkToDir(db,pathName,queue);
    cant=QueueDepth(queue);
    if( (*pids=calloc(cant+1,sizeof(int)))==NULL )
    {
        fprintf(stderr,"Error al alocar espacio en GetListDirs.\n");
        return ERROR;
    }
    while(!PQIsEmpty(queue))
    {
        aux=Dequeue(queue);
        (*pids)[i]=(int)aux;
        i++;
    }
    FreePQ(&queue);
    return OK;
}

char *
GetPIDToUserName(int pid)
{
    pqADT queue;
    char * userName=NULL;
    queue=NewPQ((void*(*)(void*))CopyString,(void*(*)(void*))FreeString);
    if(queue==NULL)
    {
        fprintf(stderr,"Error al crear la cola en TopList.\n");
        return NULL;
    }

    PIDToUserName(db,pid,queue);
    if(!PQIsEmpty(queue))
	userName=Dequeue(queue);
    
    FreePQ(&queue);
    return userName;
}


