#include "Application.h"

string *
DirList(void)
{
    return OK;
}	

int
FileAdd( fileT file, byte *data )
{
	FILE *fptr;
	
	if( FileExists(file) ){
		//CONFLICTO
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
DirAdd( string dirName  )
{
	AddDir(dirName); //jujuaju, muy ambiguo, cambiar el nombre de la func de database!
	return CopyDir(dirName, SERVER_PATH);		
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
ReqDir( string userName, string dir, fileT **files, byte ***databuffer )
{
	int nfiles;
	int i;
	
	//RegisterDirToUser(dir,userName);
	nfiles = DirFilesList(dir,files);
		
	(*databuffer) = malloc(sizeof(byte**)*nfiles);
			
	for(i=0; i<nfiles; i++) {
		if( ((*databuffer)[i] = ReqFile((*files)[i])) == NULL )
			return ERROR;
	}
	
	return nfiles;
}

int
InitApplication(void)
{
/*	if(InitBD()==ERROR)
	{
		fprintf(stderr,"Error fatal al intentar abrir la base de datos. No se puede continuar.\n");
		return ERROR;
	}

    return OK;
*/
	
}


