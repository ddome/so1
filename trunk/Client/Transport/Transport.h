#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

/* System Includes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#define COMM_DIR "/home/damian/Desktop/so1/Server/comm/"

int InitTransport(void);

#endif

