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
	return ERROR;
    
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
	return ERROR;
    
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
	return ERROR;
    }
    //Armo el archivo con la informacion que llego
    fwrite(data,sizeof(byte),GetSize(file),fptr);
    printf("%d\n",GetSize(file));		

    
    return OK;
}

int
FileRem(  fileT file )
{
    if( !FileExists(file) )
    {
	return ERROR;
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
ReqDir( string userName, string dir, fileT **files, byte ***databuffer )
{
    int nfiles;
    int i;
    
    RegisterDirToUser(dir,userName);
    nfiles = DirFilesList(dir,files);
	    
    (*databuffer) = malloc(sizeof(byte**)*nfiles);
		    
    for(i=0; i<nfiles; i++)
    {
	if( ((*databuffer)[i] = ReqFile((*files)[i])) == NULL )
	    return ERROR;
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
	return ERROR;
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
    int i=0;
    char * aux;
    GetListDirsAll(out);
    while((*out)[i]!=NULL)
    {
	aux=(*out)[i];
	free(aux);
	i++;
    }
    free(*out);
    return i;
}

int
CantUsersLinkToDir(char * pathName)
{
    if(pathName==NULL)
        return -1;
    return GetCantUsersLinkToDir(pathName);
}


