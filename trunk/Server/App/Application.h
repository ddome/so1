
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Prompt.h"
#include "../Lib/genlib.h"
#include "../Lib/defines.h"
#include "fileHandler.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_PATH "/bomsyinc/backup"

void InitApplication(void);

int NewClient(int ID,string name);
int ListDirs(int reqID, string *out);
int TopList(string *out);
int TopListUser(int userID, string *out);
void InitApplication(void);
byte* ReqFile( fileT file );
int DirAdd( string dirName  );
int FileRem( fileT file );
int FileAdd( fileT file, byte *data );

#endif