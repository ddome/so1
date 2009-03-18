
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Prompt.h"
#include "../Lib/genlib.h"
#include "../Lib/defines.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

//Por default el id del servidor prompt es 0
#define PROMPT_ID 0

//operaciones
#define EXIT		0
#define NEW_CLIENT  1
#define DIR_REQ		2 // Es atendida por un proceso dedicado
#define FILE_ADD	3
#define FILE_REM    4
#define FILE_ACT	5  // Es atendida por un proceso dedicado
#define DIR_LIST    6
#define TOP_REQ     7
#define TOP_USR_REQ 8
#define PING		9 // Envia para verificar coneccion

void StartApplication(void);

#endif