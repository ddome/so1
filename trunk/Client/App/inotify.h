#ifndef __INOTIFY__
#define __INOTIFY__

#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../Lib/genlib.h"
#include "../Lib/defines.h"
#include "./fileHandler.h"
#include "../Lib/listADT.h"
#include "../Transport/inotifyMsg.h"
#include "../Transport/OutputPipe.h"
#include "../Session/Session.h"

#define BORRAR    1
#define CREAR     2
#define RENAME    3
#define MODIFICAR 4

#define MAX_LINE 120
#define MAX_PATH 255

#define __DEFAULT_PID__ 0

#define BK_PATH_CLIENT "/home/carlos/Desktop/so1/Client/backup/"

int inotifyWatcher(process_t process);

#endif
