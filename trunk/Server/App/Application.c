#include "Application.h"

int
NewClient(int ID,string name)
{
	//ADDCLIENT(DATABASE,ID,nombre);
	return OK;
}

int
ListDirs(int reqID, string *out)
{
	//out = GetDirsName(DATABASE);
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
void
InitApplication(void)
{
  //INICIAR DATABASE
}


