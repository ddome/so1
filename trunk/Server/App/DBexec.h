#ifndef __DB_EXEC__
#define __DB_EXEC__

#include <stdio.h>
#include "../sqliteADT/sqliteADT.h"
#include "../Lib/defines.h"
#include "../Lib/genlib.h"

int InitBD(void);

int GetListDirs(const char * userName, string **out);

int AddClient(const char * userName);

int SetClientOnline(const char * userName);

int GetUserStatus(const char * nameName);

#endif
