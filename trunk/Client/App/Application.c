#include "Application.h"

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
			return ERROR;
	}
	//Armo el archivo con la informacion que llego
	fwrite(data,sizeof(byte),GetSize(file),fptr);
		
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
DirAdd( string dirName, fileT *files, byte **data, int nfiles  )
{	
	
	int i;
	
	for( i=0; i < nfiles; i++ ) {
		
		if( !DirExists(files[i].path) ) {
			if( CreateDir(files[i].path) == ERROR ){
				return ERROR;
			}
		}
		FileAdd(files[i], data[i]); 
	}
	
	return OK;
}
	
byte *
ReqFile( fileT file )
{
	FILE *fptr;
	byte *data;
	int a;
	
	if( (fptr = OpenReadFile(file)) == NULL ) {
		return NULL;
	}
	
	if( (data=malloc(a=GetSize(file))) == NULL ) {
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


