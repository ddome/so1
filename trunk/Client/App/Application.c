#include "Application.h"
#include "../Transport/OutputPipe.h"

string *
DirList(void)
{
    return NULL;
}	

int
FileAdd( fileT file, byte *data )
{
	FILE *fptr;
	
	if( FileExists(file) ){
		//RESOLVER CONFLICTO
		DeleteFile(file);
	}

	if( (fptr = CreateFile(file)) == NULL ) {
		WritePrompt("No se pudo crear el archivo");
		return ERROR;
	}

	//Armo el archivo con la informacion que llego
	fwrite(data,sizeof(byte),GetSize(file),fptr);

	fclose(fptr);
		
	return OK;
}

int
FileRem( fileT file )
{
	if( !FileExists(file) ) {
		WritePrompt("El archivo no existe");
		return ERROR;
	}
	else {
		DeleteFile(file);
	}
	
	return OK;
}

int
DirAdd( string dirName, fileT *files, byte **data, int nfiles  )
{       
	
	int i;
	int size;

	if( !DirExists(dirName) ) {
		if( CreateDir(dirName) == ERROR ){
			WritePrompt("Error al crear un directorio");
			return ERROR;
		}
	}	

	for( i=0; i < nfiles; i++ ) {
		
		if( !DirExists(files[i].path) ) {
			if( CreateDir(files[i].path) == ERROR ){
				WritePrompt("Error al crear un directorio");
				return ERROR;
			}
		}
		size = GetSize(files[i]);
		FileAdd(files[i], data[i]); 
	}
	
	return OK;
}

int
DelDir( string dir )
{   
	string dest;
	dest = Concat(SERVER_PATH,dir);
	dest = Concat(dest,"/");
	RemoveDir(dest);
	free(dest);
	
    return OK;
}

byte *
FileReq( fileT file )
{
	FILE *fptr;
	byte *data;
	int a;
	
	if( (fptr = OpenReadFile(file)) == NULL ) {
		WritePrompt("El archivo es inexistente");
		return NULL;
	}
	
	if( (data=malloc(a=GetSize(file))) == NULL ) {
		WritePrompt("Error de memoria");
		return NULL;
	}
		
	fread( data, 1, GetSize(file), fptr );
	
	return data;
}

int
InitApplication(void)
{
    return OK;	
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
    string dir;
    
    dir = Concat(SERVER_PATH,dirName);
    
    nfiles = DirFilesList(dir,files);
	
    (*databuffer) = malloc(sizeof(byte**)*nfiles);
	
    for(i=0; i<nfiles; i++)
    {
		if( ((*databuffer)[i] = ReqFile((*files)[i])) == NULL )
			return ERROR;
    }
    
    return nfiles;
}

