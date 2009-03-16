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


/* Comandos el Prompt
*/

static int RegisterDirectory(scannerADT scanner, void * data)
{
    int retValue = OK;
    
    return retValue;    
}

static int ListUsers(scannerADT scanner, void * data)
{
    int retValue = OK;
    
    return retValue;
}

static int ListUserDirectory(scannerADT scanner, void * data)
{
    int retValue = OK;
    
    return retValue;
}

static int ListLast10(scannerADT scanner, void * data)
{
    int retValue = OK;
    
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
    InsertExpression(tree, "Registrar",    RegisterDirectory);
    InsertExpression(tree, "Usuarios",   ListUsers);
    InsertExpression(tree, "Dir",        ListUserDirectory);
    InsertExpression(tree, "Last10",     ListLast10);
    InsertExpression(tree, "Salir",      ExitPrompt);
    InsertExpression(tree, "Help",   ShowCommands);
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
   
    /*Help(NULL, NULL);*/
    while ( !terminar )
    {
        printf("user@host:~ $ ");
        strAux = ReadLine( stdin );
        if ( strcmp(strAux, "Salir\n") == 0 )
            terminar = TRUE;
        else if(strcmp(strAux, "\n") != 0)
            status = ReadExpression( tree, strAux, NULL );
        else
            status = 0;
        if(status < 0)
            fprintf(stderr, "Comando invalido.\n");

        free(strAux);
    }
    FreeTree(tree);
/* Aca poner un broadcast avisando a los clientes q el servidor se va de vacaciones*/
}

