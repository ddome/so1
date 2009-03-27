#ifndef __SERVER_H__
#define __SERVER_H__

/*
*  System Includes
*/

/*
*  Project Includes
*/
#include "../Session/Session.h"

/*
*  Defines
*/

#define __DEFAULT_PID__	  0
#define __SPAWN_PROMPT__  1
#define __SPAWN_DIR__     2
#define __SPAWN_DEMAND__  3

void StartListening(void);

pid_t ReadRequest(void* data);

#endif