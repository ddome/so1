/*
 *  genlib.c
 *  
 *
 *  Created by Damian Dome on 3/15/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "genlib.h"

string 
Concat( string s1, string s2 )
{
	string aux;
	
	if( (aux=malloc(strlen(s1)+strlen(s2))) == NULL )
		return NULL;	
	strcpy(aux,s1);
	strcat(aux,s2);
	return aux;
}

string
CreateString( string copy )
{
	string aux;
	
	if( (aux=malloc(strlen(copy))) == NULL )
		return NULL;
	strcpy(aux,copy);
	return aux;
}