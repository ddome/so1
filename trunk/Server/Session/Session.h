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

//	Manda una senial esperando contestacion a un user


boolean SendPingSignal(int userID);

// Manda un archivo nuevo (en el file se expecifica su directorio)

boolean	SendFileAdd(int userID, fileT * file, void *fileData);

// Manda un pedido de borrarlo del sistema (cada cliente lo borra por su cuenta)

boolean SendFileRem(int userID, fileT * file);

boolean SendFileChange(int userID fileT * file, void *fileData);

boolean TopListRequest(int *nitems,string *list);

boolean TopUserListRequest(int *nitems, string *list);

// pide al server un directorio, el server se encarga de mandar SendFileAdd de cada file dentro de el

boolean DirRequest(string path);

#endif