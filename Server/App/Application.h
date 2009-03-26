
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Prompt.h"
#include "../Lib/genlib.h"
#include "../Lib/defines.h"
#include "fileHandler.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_PATH "~/bomsyinc/backup"
#include "../sqliteADT/sqliteADT.h"
#include "../Lib/pqADT.h"
#include "DBexec.h"


int NewClient(string name);
int ListDirs( const char * userName,string **out);
int TopList(string *out);
int TopListUser(int userID, string *out);
int InitApplication(void);
byte* ReqFile( fileT file );
int DirAdd( string dirName  );
int FileRem( fileT file );
int FileAdd( fileT file, byte *data );

int ReqDir( string userName, string dir, fileT **files, byte ***databuffer );


#endif
