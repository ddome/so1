/*
 *  fileHandler.c
 *  
 *
 *  Created by Damian Dome on 3/11/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "fileHandler.h"

#define FILE_T 0x8
#define DIR_T 0x4

string
GetPath( fileT file )
{
	string aux1,aux2;
	
	aux1 = Concat(file.path, "/");
	aux2 = Concat(aux1,file.fName);
	
	//free(aux1);
	return aux2;
}


fileT
NewFileT( string dir, string fName )
{
	fileT aux;
	string path;
	
	strcpy(aux.path,dir);
	strcpy(aux.fName,fName);
	
	path = GetPath(aux);
    stat(path,&(aux.sb));
//	free(path);
	
	return aux;
}

int
GetSize(fileT file)
{
	return file.sb.st_size;
}


static string *
MakeFilesArray(fileT *files, int nfiles)
{
	string *array;
	int i;
	
	if( (array=malloc(sizeof(string)*nfiles)) == NULL )
		return NULL;
	
	for( i=0; i<nfiles; i++ ) {
		array[i] = Concat(files[i].path,files[i].fName);
	}
	
	return array;
}

int
FilesWatch( string *path, int nfile )
{
	int i;
	struct stat sb;
	time_t last_time[MFILE+1];
	
	//Inicializacion
	for ( i=0; i < nfile; i++ ) {
		
		if( stat(path[i],&sb) == -1 )
			return ERROR;
		
		last_time[i] = sb.st_mtime;
	}	
	
	
	//Loop que controla cada uno de los archivos
	//El ciclo termina cuando cambia un archivo
	for(;;) {
        
		for( i=0; i < nfile; i++ ) {
			if( stat(path[i],&sb) == -1 || sb.st_mtime != last_time[i] )
				return i;
				
			//Espera para volver a controlar
			sleep(SLEEP_TIME);
			
			printf("Desperto\n");
		}
	}	
}

int
InitFilesStat(fileT *files, int nfile)
{
	int i;
	string path;
	
	
	for ( i=0; i < nfile; i++ ) {
			
		path = GetPath(files[i]);
		if( stat(path,&(files[i].sb)) == -1 )
			return ERROR;
		free(path);
	}
	return OK;
}
	
int
FilesHasChanged( fileT *files, int nfile )
{
	int i;
	string aux;
	struct stat sb;
	
	
	for( i=0; i < nfile; i++ ) {
		aux = Concat(files[i].path, files[i].fName);
		if( stat(aux,&sb) == -1 || sb.st_mtime != files[i].sb.st_mtime ) {
			files[i].sb = sb;
			return i;
		}
		free(aux);
	}
	return -1;
}


FILE *
CreateFile( fileT file )
{
	string path;
	FILE *fptr;
	
	if( FileExists(file) == TRUE )
		return NULL;
	
	path = GetPath(file);
	
	fptr=fopen(path,"wb+");
	free(path);
	return fptr;
}

int
DeleteFile( fileT file )
{
	string path;
	
	if( FileExists(file) == FALSE )
		return ERROR;
	
	path = GetPath(file);
	
	if( remove(path) != 0  ) {
		free(path);
		return ERROR;
	}
	
	free(path);
	return OK;
}
	   
boolean 
FileExists( fileT file )
{
	string path;
	FILE * fptr;
	
	path = GetPath(file);
	
	if( (fptr=fopen(path,"r")) == NULL ) {
		free(path);
		return FALSE;
	}
	free(path);
	fclose(fptr);
	return TRUE;
}

int
CopyFile( string srcFile, string destFile )
{
	FILE *sptr;
	FILE *dptr;
	byte aux;
	
	if( (sptr=fopen(srcFile,"rb")) == NULL ) {
		return ERROR;
	}
	if( (dptr=fopen(destFile,"wb")) == NULL ) {
		return ERROR;
	}
	
	while( fread( &aux, 1, 1, sptr ) != 0 ) {
		fwrite( &aux, 1, 1, dptr );
	}
	
	fclose(sptr);
	fclose(dptr);
	
	return OK;
}
	
int 
CopyDir( string srcDir, string destDir  )
{
	DIR *sptr;
	//FILE *faux;
	struct dirent *d;
	string aux1,aux11;
	string aux2,aux22;
	
	if ( (sptr = opendir(srcDir)) == NULL ) {
		return ERROR;
	}
	
	mkdir(destDir,0777);
	while( (d = readdir(sptr)) ) {
		if( (d->d_ino != 0) && (strcmp(d->d_name,"..") != 0) && (strcmp(d->d_name, ".") != 0) ) {
			if( d->d_type == FILE_T ) {
				aux1 = Concat( aux11 = Concat(srcDir,"/"),d->d_name);
				aux2 = Concat( aux22 = Concat(destDir,"/"),d->d_name);
				CopyFile( aux1, aux2 );
				free(aux1);
				free(aux2);
				free(aux11);
				free(aux22);
			}
			else {
				aux1 = Concat( aux11 = Concat(srcDir,"/"),d->d_name);
				aux2 = Concat( aux22 = Concat(destDir,"/"),d->d_name);
				CopyDir( aux1, aux2 );
				free(aux1);
				free(aux2);
				free(aux11);
				free(aux22);
			}
		}	
	}
	
	closedir(sptr);
	return OK;
}
	
FILE *
OpenFile( fileT file )
{
	string path;
	FILE *fptr;
	
	path = GetPath(file);
	fptr=fopen(path,"wb+");
	
	free(path);
	return fptr;
}

FILE *
OpenReadFile( fileT file )
{
	string path;
	FILE *fptr;
	
	path = GetPath(file);
	fptr=fopen(path,"rb");
	
	free(path);
	return fptr;
}

static int
pDirFilesList( string dir, fileT *files, int *pos )
{
	DIR *dptr;
	int nfiles;
	struct dirent *d;
	string aux1;
	string aux2;
	
	if ( (dptr = opendir(dir)) == NULL ) {
		return ERROR;
	}
	
	nfiles = 0;
	while( (d=readdir(dptr)) ) {
		if( d->d_ino != 0 && strcmp(d->d_name,"..") != 0 && strcmp(d->d_name, ".") != 0)  {
			if( d->d_type == FILE_T ) {
				files[*pos] = NewFileT(dir, d->d_name);
				nfiles++;
				*pos = *pos + 1;
			}
			else {
				aux1 = Concat( aux2 = Concat(dir,"/"),d->d_name);
				nfiles += pDirFilesList( aux1, files, pos );
				free(aux1);
				free(aux2);
			}
		}	
	}
	
	return nfiles;
}

int
DirFilesNumber( string dir )
{
	DIR *dptr;
	int nfiles;
	struct dirent *d;
	string aux1;
	string aux2;
	
	if ( (dptr = opendir(dir)) == NULL ) {
		return ERROR;
	}
	
	nfiles = 0;
	while( (d=readdir(dptr)) ) {
		if( d->d_ino != 0 && strcmp(d->d_name,"..") != 0 && strcmp(d->d_name, ".") != 0)  {
			if( d->d_type == FILE_T ) {				
				nfiles++;
			}
			else {
				aux1 = Concat( aux2 = Concat(dir,"/"),d->d_name);
				nfiles += DirFilesNumber( aux1  );
				free(aux1);
				free(aux2);
			}
		}	
	}
	
	return nfiles;
}

int
DirFilesList(string dir, fileT **files)
{
	int nfiles;
	int aux=0;
	
	nfiles  = DirFilesNumber(dir);
	*files = malloc(sizeof(fileT)*nfiles);
	
	pDirFilesList(dir,*files,&aux);		
									
	return nfiles;
}

static int
pDirPathList( string dir, string * dirList, int *pos )
{
	DIR *dptr;
	int ndirs;
	struct dirent *d;
	string aux1;
	string aux2;
	
	if ( (dptr = opendir(dir)) == NULL ) {
		return ERROR;
	}
	
	ndirs = 0;
	while( (d=readdir(dptr)) ) {
		if( d->d_ino != 0 && strcmp(d->d_name,"..") != 0 && strcmp(d->d_name, ".") != 0)  {
			if( d->d_type != FILE_T ) {	
				aux1 = Concat( aux2 = Concat(dir,"/"),d->d_name);
				printf("%s\n",aux1);
				dirList[*pos] = CreateString(aux1);
				*pos = *pos + 1;
				ndirs += pDirPathList( aux1, dirList, pos );
				free(aux1);
			}
		}	
	}
	
	return ndirs;
}

int
DirsNumber( string dir )
{
	DIR *dptr;
	int ndirs;
	struct dirent *d;
	string aux1;
	string aux2;
	
	if ( (dptr = opendir(dir)) == NULL ) {
		return ERROR;
	}
	
	ndirs = 0;
	while( (d=readdir(dptr)) ) {
		if( d->d_ino != 0 && strcmp(d->d_name,"..") != 0 && strcmp(d->d_name, ".") != 0)  {
			if( d->d_type != FILE_T ) {
				aux1 = Concat( aux2 = Concat(dir,"/"),d->d_name);
				ndirs += (DirFilesNumber( aux1 ) + 1);
				free(aux1);
				free(aux2);
			}
		}	
	}
	
	return ndirs;
}

	
int
DirPathList(string dir, string **dirList)
{
	int ndirs;
	int aux=0;
	
	ndirs   = DirsNumber(dir);
	*dirList = malloc(sizeof(string)*ndirs);
	
	pDirPathList(dir,*dirList,&aux);		
									
	return ndirs;
}

	
boolean 
DirExists( string dirPath )
{
	DIR *dptr;
	
	if( (dptr = opendir(dirPath)) == NULL ) {
		return FALSE;
	}
	else {
		closedir(dptr);
		return TRUE;
	}	
}

int
CreateDir( string dir )
{
	if( mkdir(dir,0777) != 0 ) {
		return ERROR;
	}
	
	return OK;
}
	
string
DirName(string dirPath)	
{
	string dirName;
	printf("%s\n",dirPath);
	dirName = strrchr(dirPath, '/');
	printf("Dir: (%s)\n",(dirName+1));
	return dirName+1;	
}	
	
int
RemoveDir( string dir )
{
	DIR *dptr;
	struct dirent *d;
	string aux1;
	string aux2;
	int ret;
	ret = OK;
	if ( (dptr = opendir(dir)) == NULL ) {
		return ERROR;
	}

	while( (d=readdir(dptr)) ) {
		if( d->d_ino != 0 && strcmp(d->d_name,"..") != 0 && strcmp(d->d_name, ".") != 0)  {
			if( d->d_type == DIR_T ) {
				aux1 = Concat( dir, aux2 = Concat(d->d_name,"/"));
				ret = RemoveDir( aux1 );
				free(aux1);
				free(aux2);
			}
			else {
				aux1 = Concat(dir,d->d_name);
				remove(aux1);
				free(aux1);
			}
		}	
	}
	
	rmdir(dir);
	return ret;
}	
	
	

