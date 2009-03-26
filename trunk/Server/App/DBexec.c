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
InitBD()
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
AddClient(const char * userName)
{
    int ret;
    int boolRet;
    ret=AddUser(db, userName);
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
/*
static int
GetListUsersByID(int ** userID)
{
    int exit;
    int i,cant;
    char * auxStr;
    int * userIDs;
    pqADT queue;
    
    queue=NewPQ((void*(*)(void*))sCopia,(void*(*)(void*))sLibera);
    if(ShowOnlineByID(db,queue)!=DB_SUCCESS)
    {
	fprintf(stderr,"Error fatal al intentar abrir la base de datos. No se puede continuar.\n");
	return ERROR;
    }
    
    if( (*userID=malloc(QueueDepth(queue)*sizeof(int)))==NULL ):
    {
	fprintf(stderr,"Error fatal al intentar abrir la base de datos. No se puede continuar.\n");
	return ERROR;
    }
    FreePQ(&queue);
    return (exit = TRUE);
}*/













