#include "Application.h"

boolean reqFlag = FALSE;
int opdesc;
int IDdesc = PROMPT_ID;
int counter = 0;
static void
SIMULADOR()
{
	counter ++;
	
	if( counter == 1000 ){
		reqFlag = TRUE;
		opdesc = 1;
		IDdesc = 1;
	}
	
	if( counter == 2000 ){
		reqFlag = TRUE;
		opdesc = 1;
		IDdesc = 2;
	}

	if( counter == 10000 ){
		reqFlag = TRUE;
		opdesc = 0;
		IDdesc = PROMPT_ID;
	}
}

//Funciones a utilizar
static int
EndApp()
{
	int exit;
	int * userIDs;
	/* 
	 userIDs = GETLISTOFUSERS(DATABASE);
	 for(i=0; i<cantidad de usuarios; i++ )
		SendEndSignal(userIDs[i]);
	 */
	return (exit = TRUE);
}


static boolean
NewClient(int reqID,int *data) //ahora es un int data, deberia ser una struct con toda la info
{
	int exit;
	//ADDCLIENT(DATABASE,reqID,data.nombre,data.otrainfo);
	return (exit = FALSE);
}

static boolean
ListDirs(int reqID)
{
	int nDir = 5; //ahora es fijo, deberia chequear en la base de datos
	string *dirs;
	
	//dirs = GetDirsName(DATABASE);
	//SendDirList(reqID,dirs); funcion de la session para mandar la data
	
	return FALSE;
}	

static boolean
TopList()
{
	string *list;
	
	//list = GetTopList(ALL,DATABASE);
	//SendTopList(reqID,list,PROMPT_ID); funcion de session
	
	return FALSE;
}

static boolean
TopListUser(int *data)
{
	string *list;
	int userID;
	
	//userID = *data;
	//list = GetTopList(userID, DATABASE);
	//SendTopList(reqID,list,PROMPT_ID); funcion de session
	
	return FALSE;
}

static void 
ReadOp(int *op,int *reqID,void *data)
{
	//aca llamaria a la funcion de sesion
	while( reqFlag == FALSE ) { 
		SIMULADOR(); // funcion de debugueo, cuenta y manda entradas como si vinieran de afuera
	}	

	*op = opdesc;
	*reqID = IDdesc;
	*(int *)data = 1; //fruta
	reqFlag = FALSE; //vacio el pedido
}

//Aca esta la posta
static boolean
Serve(int op,int reqID, void *data)
{	
	printf("Operacion recibida %d de usuario %d\n", op, reqID );
	
	switch( op ) {
		case EXIT: 
			return EndApp();
			break;
		case NEW_CLIENT:
			return NewClient(reqID,data);
			break;
		case TOP_REQ:
			return TopList();
			break;
		case TOP_USR_REQ:
			return TopListUser(data);
		default:
			fprintf(stderr,"Operacion no definida\n"); //solo para debug
			return FALSE;
	}
}

void
StartListening(void)
{
	int op;
	int reqID;
	void *data;
	boolean exitFlag;
	
	//solo para debug
	data = malloc(sizeof(int));
	
	exitFlag = FALSE;
    while( exitFlag == FALSE ) {
		ReadOp(&op,&reqID,data);
		exitFlag = Serve(op, reqID, data);
	}

}

void
StartApplication(void)
{
 
/*	pid_t pid;
    switch( pid=fork() ) { 
		case -1:
			fprintf(stderr,"Error en el fork()\n");
			break; 
		case 0:
			//Comienzo del proceso de servidor 
			StartListening();
			break;
		default:
			//Inicio del front-end del servidor
			Prompt();
			break;
	}	
*/
	StartListening();
	
}


