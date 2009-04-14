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
#include "../Lib/defines.h"
#include "Application.h"
#include "../Transport/OutputPipe.h"


#define TRUE 1
#define FALSE 0
#define OK 1


#define MAX_LINE 120

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

static int RegisterDirectory(scannerADT scanner, void * data)
{
    int retValue = OK;
    char * aux;
    if(MoreTokensExist(scanner)) {
		aux=ReadToken(scanner);
		if(DirAdd(aux)!=OK) {
			retValue=ERROR;
		}
		free(aux);
    }
    else
    {
		retValue=ERROR;
    }

    return retValue;    
}

static int ListUsers(scannerADT scanner, void * data)
{
    int retValue = OK;
    if(MoreTokensExist(scanner))
    {
		fprintf(stderr,"El comando no admite argumentos.\n");
		retValue=ERROR;
    }
    else
    {
		UserList();
    }
    
    return retValue;
}

static int ListUserDirectory(scannerADT scanner, void * data)
{
    int retValue = OK;
    char * aux;
    if(!MoreTokensExist(scanner))
    {
		fprintf(stderr,"El comando espera como argumento un nombre de usuario.\n");
		retValue=ERROR;
    }
    else
    {
		aux=ReadToken(scanner);
		ListDirs(aux);
		free(aux);
    }

    return retValue;
}

static int ListLast10(scannerADT scanner, void * data)
{
    int retValue = OK;
    char * aux;
    if(MoreTokensExist(scanner))
    {
		aux=ReadToken(scanner);
		TopListUser(aux);
		free(aux);
    }
    else
    {
		TopList();
    }
    
    return retValue;
}


static int ExitPrompt(scannerADT scanner, void * data)
{
    int retValue = OK;
    
    return retValue;
}

static int ShowCommands(scannerADT scanner, void * data)
{
    printf("\nBombSync - Server\n");
    printf("==================\n");
    printf("Inicializando Aplicacion...\n");
    printf("Inicializando Sesion...\n");
    printf("Inicializando Aplicacion...\n");
    printf("==================\n");
    printf("Comandos disponibles:\n");

    return OK;
}

/* Se cargan los comandos en el arbol de expresiones
*/

static void LoadTree(treeADT tree)
{
    InsertExpression(tree, "Registrar",  RegisterDirectory);
    InsertExpression(tree, "Usuarios",   ListUsers);
    InsertExpression(tree, "Dir",        ListUserDirectory);
    InsertExpression(tree, "Last10",     ListLast10);
    InsertExpression(tree, "Salir",      ExitPrompt);
    InsertExpression(tree, "Help",       ShowCommands);
}

/* Prompt
*/

void
Prompt(void)
{
    char * strAux;
    int status = OK;
    int terminar = FALSE;
    treeADT tree;
    tree = NewTree();
    LoadTree(tree);

    while ( !terminar )
    {
        printf("user@server:~ $ ");
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
  /*  GoodBye();*/
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
	



