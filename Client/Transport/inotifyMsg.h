#ifndef __INOTIFY_MSG_H__
#define __INOTIFY_MSG_H__

/* System Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Project Includes
*/

#include "Transport.h"
#include "../Lib/defines.h"

/* Defines de los Fifo's a utilizar
*/

#define __DEFAULT_FIFO_MODE__ 0777

#define INOT "IN"

/*
*  Posibles mensajes al INotify
*/

#define __INOTIFY_NO_DATA__ '&'
#define __INOTIFY_ENABLE__  '+'
#define __INOTIFY_DISABLE__ '-'
#define __INOTIFY_EXIT__    '$'

/* Funciones
*/

int InitINotifyMsg(pid_t pid);

int WriteINotifyMsg(char msg);

char ReadINotifyMsg(void);

void CloseINotifyMsg(void);

#endif



