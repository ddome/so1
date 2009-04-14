/*
 *    System includes
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 *    Proyect includes
 */
#include "../Lib/scannerADT.h"
#include "../Lib/tree.h"
#include "../Session/Session.h"
#include "../Transport/OutputPipe.h"

#define TRUE 1
#define FALSE 0
#define OK 1
#define RETURN 2

#define MAX_LINE 120

/*
*  Variables globales
*/

char name[MAX_LINE];

/* Funciones de utilidad
*/

static char * ReadLine( FILE * inputFile )
{
    char line[MAX_LINE+ 1];
    char * resp;
    int len;

    if( fgets( line, sizeof(line), inputFile ) ==NULL )
            return NULL;
    if( ((len = strlen( line )) == 0) )
        return NULL;
    line[len-1] = '\n';
    line[len] = '\0';

    resp = malloc( (len + 1) * sizeof(char) );
    strncpy(resp, line, len + 1);

    return (resp);
}


/* Comandos del Prompt
*/

static int Server(scannerADT scanner, void * data)
{
    int retValue = ERROR;
    if(!MoreTokensExist(scanner))
    {
      if(SendConectionSignal(getppid())==OK)
      {
        retValue=OK;
      }
    }
    
    return retValue;    
}

static int Name(scannerADT scanner, void * data)
{
    int retValue = ERROR;
    char * aux;
    if(MoreTokensExist(scanner))
    {
        aux = ReadToken(scanner);
        if(!MoreTokensExist(scanner) && SendNewClientSignal( aux, getppid()) == OK)
        {
			strcpy(name, aux);
            retValue = OK;
        }
		free(aux);
    }
    return retValue;
}

static int ListSincDirs(scannerADT scanner, void * data)
{
    int retValue = ERROR;
	
	if(!MoreTokensExist(scanner))
	{
		SendDirListReq(name);
		retValue = OK;
	}
    return retValue;
}

static int AddDir(scannerADT scanner, void * data)
{
    int retValue = ERROR;
	char * aux;
	if(MoreTokensExist(scanner))
	{
		aux = ReadToken(scanner);
		if(!MoreTokensExist(scanner) && SendDirReq(name, getppid(), aux) == OK)
		{
			retValue = OK;
		}
		free(aux);
	}
    return retValue;
}


static int RemDir(scannerADT scanner, void * data)
{
    int retValue = OK;
    
    return retValue;
}

static int Exit(scannerADT scanner, void * data)
{
    int retValue = ERROR;
    if(!MoreTokensExist(scanner))
    {
      if(SendConectionSignal(getppid())==OK)
      {
        retValue=OK;
      }
    }

    return retValue;
}

static int Help(scannerADT scanner, void * data)
{


  return OK;
}

/* Se cargan los comandos en el arbol de expresiones
*/

static void LoadTree(treeADT tree)
{
    InsertExpression(tree, "Servidor",    Server);
    InsertExpression(tree, "Nombre",      Name);
    InsertExpression(tree, "Lista",       ListSincDirs);
    InsertExpression(tree, "Agregar",     AddDir);
    InsertExpression(tree, "Remover",     RemDir);
    InsertExpression(tree, "Salir",       Exit);
    InsertExpression(tree, "Help",        Help);
}

/* Prompt
*/

int
Prompt(void)
{
    char * strAux;
    int status = OK;
    int terminar = FALSE;
    treeADT tree;
    tree = NewTree();
    LoadTree(tree);
	strcpy(name, "");
    while ( !terminar )
    {
        printf("user@client:~ $ ");
        strAux = ReadLine( stdin );

        if(strcmp(strAux, "\n") != 0)
            status = ReadExpression( tree, strAux, NULL );
        else
            status = 0;
         if(status < 0)
            fprintf(stderr, "Comando invalido.\n");

        free(strAux);
    }
    FreeTree(tree);
    return status == ERROR ? ERROR : RETURN;
}

void
PromptReader(void)
{
	string msg;
	
	do {
		msg = ReadMessage();
		
		if(	strcmp(msg,"$SALIR") != 0 ) { 
			printf("%s\n",msg);
		}
	}while( strcmp(msg,"$SALIR") != 0);	
	
}	