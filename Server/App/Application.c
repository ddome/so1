#include "Application.h"

int
NewClient(string name,int pid)
{
    int ret=AddClient(name,pid);
    if(ret==OK)
    {
	SetClientOnline(name);
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
ListDirs( string userName,string **out)
{
    GetListDirs(userName,out);
    
    return OK;
}

int
LogAction(const char * userName,const char * fileName,const char * action)
{
    return NewLogEntry(userName,fileName,action);
}


int
TopList(string **out)
{
    int ret;
    ret=GetTopList(NULL,out);
    if(ret==ERROR)
	return ERROR;
    
    return OK;
}

int
TopListUser(string userName, string **out)
{
    int ret;
    ret=GetTopList(userName,out);
    if(ret==ERROR)
	return ERROR;
    
    return OK;
}

int 
UserList( string **out )
{
    int ret;
    ret=GetUserOnlineList(out);
    if(ret==ERROR)
	return ERROR;
    
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
FileRem( fileT file )
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
DirAdd( string dirName  )
{
    NewDir(dirName);
    return CopyDir(dirName, SERVER_PATH);		
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
    
    //RegisterDirToUser(dir,userName);/*Porq esta comentado?*/
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
    FreeDatabase();
    return OK;
}

int
DirconnectUser(string userName)
{
    SetClientOffline(userName);
    return OK;
}



