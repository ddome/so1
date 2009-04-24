/* System includes */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

/* Project includes */


#include "Application.h"
#include "Prompt.h"


int
NewClient(int pid)
{
    int ret=AddClient(pid);
    if(ret==OK)
    {
		return NEW_USR_OK;
    }
		else if(ret==ERROR)
    {
	//USUARIO YA CONECTADO
		return NEW_USR_EXIST;
    }
    else
		return NEW_USR_ERROR;
}

int
ConnectUser(int pid,char * userName)
{
    int ret;
    ret=SetName(pid,userName);
    if(ret==OK)
		return NEW_USRNAME_OK;
    else if(ret==ERROR)
		return NEW_USRNAME_EXIST;
    else
		return NEW_USRNAME_ERROR;
	
    return OK;
}

int
ListDirs( string userName)
{
    char ** out;
    int i=0;
    char * aux;
    GetListDirs(userName,&out);
    printf("Los directorios registrador al usuario %s son:\n",userName);
    printf("==============================================\n");
    while( out[i] != NULL )
    {
		aux=out[i];
		printf("%s\n",aux);
		free(aux);
		i++;
    }
    free(out);
    
    return OK;
}

int
LogAction(const char * userName,const char * fileName,const char * action)
{
    return NewLogEntry(userName,fileName,action);
}


int
TopList(void)
{
    int ret;
    char ** out;
    int i=0;
    char * aux;
    ret=GetTopList(NULL,&out);
    if(ret==ERROR)
	return __ERROR_APPLICATION__;
    
    printf("Las ultimas diez acciones realizadas fueron:\n");
    printf("============================================\n");
    while( out[i] != NULL )
    {
	aux=out[i];
	printf("%s\n",aux);
	free(aux);
	i++;
    }
    free(out);
    
    return OK;
}

int
TopListUser(string userName)
{
    int ret;
    int i=0;
    char ** out;
    char * aux;
    ret=GetTopList(userName,&out);
    if(ret==ERROR)
      return __ERROR_APPLICATION__;
    
    printf("Las ultimas diez acciones realizadas por %s fueron:\n",userName);
    printf("============================================\n");
    while( out[i] != NULL )
    {
		aux=out[i];
		printf("%s\n",aux);
		free(aux);
		i++;
    }
    free(out);
    
    return OK;
}

int 
UserList( void )
{
    int ret;
    int i=0;
    char * aux;
    char ** out;
    ret=GetUserOnlineList(&out);
    if(ret==ERROR)
	return ERROR;
    
    printf("Usuarios conectados en este momento al servidor:\n");
    printf("================================================\n");
    while( out[i] != NULL )
    {
	aux=out[i];
	printf("%s\n",aux);
	free(aux);
	i++;
    }
    free(out);
    
    return OK;
}

int
FileAdd( fileT file, byte *data )
{
    FILE *fptr;
		
    if( FileExists(file) )
    {
	//CONFLICTO
		DeleteFile(file);
    }

    if( (fptr = CreateFile(file)) == NULL )
    {
      return __ERROR_APPLICATION__;
    }
    //Armo el archivo con la informacion que llego
    fwrite(data,sizeof(byte),GetSize(file),fptr);		

    fclose(fptr);    

    return OK;
}

int
FileRem(  fileT file )
{
    if( !FileExists(file) )
    {
      return __ERROR_APPLICATION__;
    }
    else
    {
	DeleteFile(file);
    }
    
    return OK;
}


int
DirAdd( string dirPath  )
{
    string destPath;
    int ret;
    
    
    NewDir(DirName(dirPath));
    CreateDir(destPath=Concat(SERVER_PATH,DirName(dirPath)));

    ret = CopyDir(dirPath,destPath);		
    free(destPath);
    return ret;
}
	
byte *
ReqFile( fileT file )
{
    FILE *fptr;
    byte *data;
    int a;
    
    if( (fptr = OpenReadFile(file)) == NULL )
    {
		return NULL;
    }
    
    if( (data=malloc(a=GetSize(file))) == NULL )
    {
		return NULL;
    }
    
    
    fread( data, 1, GetSize(file), fptr );
    
    return data;
}

int
ReqDir( string userName, string dirName, fileT **files, byte ***databuffer )
{
    int nfiles;
    int i;
    int ret;
    string dir;
    dir = Concat(SERVER_PATH,dirName);
    
    ret=RegisterDirToUser(dirName,userName);
    nfiles = DirFilesList(dir,files);
	    
    (*databuffer) = malloc(sizeof(byte**)*nfiles);
		    
    for(i=0; i<nfiles; i++)
    {
		if( ((*databuffer)[i] = ReqFile((*files)[i])) == NULL )
		  return __ERROR_APPLICATION__;
    }
    
    return nfiles;
}

int
DelDir( string userName, string dir )
{   

    UnRegisterDirFromUser(dir,userName);
	
    return OK;
}

int
InitApplication(void)
{	
    if(InitBD()==ERROR)
    {
	fprintf(stderr,"Error fatal al intentar abrir la base de datos. No se puede continuar.\n");
	return __ERROR_DATABASE__;
    }

    return OK;
}

int
CloseApplication(void)
{
    SetAllClientsOffline();
    FreeDatabase();
    return OK;
}

int
DisconnectUser(string userName)
{
    SetClientOffline(userName);
    return OK;
}

int
ListAllSyncDirs( string **out)
{
    int cant=0;
    cant=GetListDirsAll(out);
    if(cant==ERROR)
      return __ERROR_APPLICATION__;
    return cant;
}

int
CantUsersLinkToDir(char * pathName)
{
    if(pathName==NULL)
      return __ERROR_APPLICATION__;
    return GetCantUsersLinkToDir(pathName);
}

int *
PIDsLinkToDir(char * pathName)
{
    int * pids,cant=0;
    cant=GetCantUsersLinkToDir(pathName);
    if( (pids=calloc(cant+1,sizeof(int))) ==NULL )
        return NULL;
    
    GetListPIDsLinkToDir(pathName,&pids);
    return pids;
}

char *
ConvertPIDToUserName(int pid)
{
    return GetPIDToUserName(pid);
}














