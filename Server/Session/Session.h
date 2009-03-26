/*
 *  Session.h
 *  server
 *
 *  Created by Damian Dome on 3/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SESSION_H
#define _SESSION_H

#include "../Lib/genlib.h"
#include "../Lib/defines.h"
#include "../App/fileHandler.h"

//Por default el id del servidor prompt es 0
#define PROMPT_ID 0

//operaciones para armado del paquete
#define EXIT		0
#define NEW_CLIENT  1
#define DIR_REQ		2 // Es atendida por un proceso dedicado
#define FILE_ADD	3
#define FILE_REM    4
#define FILE_UPD	5  // Es atendida por un proceso dedicado
#define DIR_LIST    6
#define PING		9 // Envia para verificar coneccion

//	Manda una senial

boolean SendPingSignal(int userID);

// Manda un archivo nuevo (en el file se expecifica su directorio)

boolean	SendFileAdd(int userID, fileT * file, void *fileData);

// Manda un pedido de borrarlo del sistema (cada cliente lo borra por su cuenta)

boolean SendFileRem(int userID, fileT * file);

boolean SendFileChange(int userID, fileT * file, void *fileData);

boolean TopListRequest(int *nitems,string *list);

boolean TopUserListRequest(int *nitems, string *list);

// pide al server un directorio, el server se encarga de mandar SendFileAdd de cada file dentro de el

boolean DirRequest(string path);

#endif