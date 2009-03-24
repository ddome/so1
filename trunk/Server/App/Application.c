#include "Application.h"



int
NewClient(string name)
{
    int ret;
/*    if(ret==DB_ALREADY_EXISTS)
	printf("El Directorio ya existia.\n");
    else if(ret==DB_INTERNAL_ERROR)
	printf("Error interno de la base de datos.\n");
    else
	printf("Directorio agregado satisfactoriamente.\n");*/
    ret=AddClient(name);
    if(ret==OK)
	SetClientOnline(name);
    else
	printf("El usuario ya esta conectado.\n");
    return ret;
}

int
ListDirs( const char * userName,string **out)
{
    GetListDirs(userName,out);
    
    return OK;
}	

int
TopList(string *out)
{	
	//out = GetTopList(userID, DATABASE);
	return OK;
}

int
TopListUser(int userID, string *out)
{
	//out = GetTopList(userID, DATABASE);
	
	return OK;
}

int
FileAdd( fileT file, byte *data )
{
	FILE *fptr;
	
	if( FileExists(file) ){
		//CONFLICTO
	}
	else {
		if( (fptr = CreateFile(file)) == NULL ) {
			return ERROR;
		}
		//Armo el archivo con la informacion que llego
		fwrite(data,sizeof(byte),GetSize(file),fptr);
		printf("%d\n",GetSize(file));
	}
	
	return OK;
}

int
FileRem( fileT file )
{
	if( !FileExists(file) ) {
		return ERROR;
	}
	else {
		DeleteFile(file);
	}
	
	return OK;
}


int
DirAdd( string dirName  )
{
	return CopyDir(dirName, SERVER_PATH);		
}
	
byte *
ReqFile( fileT file )
{
	FILE *fptr;
	byte *data;
	
	if( (fptr = OpenReadFile(file)) == NULL ) {
		return NULL;
	}
	
	if( (data=malloc(GetSize(file))) == NULL ) {
		return NULL;
	}
	
	fread( data, 1, GetSize(file), fptr );
	printf("%d\n",GetSize(file));
	
	return data;
}

int
ReqDir( string dir, fileT *files, byte **databuffer )
{
	
	
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


