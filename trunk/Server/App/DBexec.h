#ifndef __DB_EXEC__
#define __DB_EXEC__

#include <stdio.h>
#include "../sqliteADT/sqliteADT.h"
#include "../Lib/defines.h"
#include "../Lib/genlib.h"
#include "../Lib/pqADT.h"

#define DB_ERROR -5

int InitBD(void);

int GetListDirs(const char * userName, char ***out);

int AddClient(const char * userName, int pid);

int SetClientOnline(const char * userName);

int SetClientOffline(const char * userName);

int SetAllClientsOffline(void);

int GetUserStatus(const char * nameName);

int NewDir(const char * pathName);

int RegisterDirToUser(const char * pathName,const char * userName);

int UnRegisterDirFromUser(const char * pathName,const char * userName);

int NewLogEntry(const char * userName,const char * fileName,const char * action);

int GetTopList(const char * userName,char ***out);

int GetUserOnlineList( char *** out );

int FreeDatabase(void);

#endif
