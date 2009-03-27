/*
*  Project Includes
*/

#include "Session.h"

/*
*  Functions
*/

typedef struct{
    int dni;
    int esgay;
} cacaT;

int
InitCommunication(pid_t pid)
{
    return InitIPC(pid);
}

pid_t GetRequest(void * data)
{
    return ReadIPC(data);
}

void 
ProcessRequest(void * data, pid_t requestPid)
{
	printf("llego un mensaje!: %d - %s es gay \n", ((cacaT*)data)->dni,((cacaT*)data)->esgay?"SI":"NO");

}

/***************************************************************************/
static int
EndApp()
{
	int exit;
	/*int * userIDs;*/
	/* 
	 userIDs = GETLISTOFUSERS(DATABASE);
	 for(i=0; i<cantidad de usuarios; i++ )
	 SendEndSignal(userIDs[i]);
	 */
	return (exit = TRUE);
}

static void 
ReadOp(int *op,int *reqID,void *data)
{
	
}
/*

static boolean
Serve(int op,int reqID, void *data)
{	
	printf("Operacion recibida %d de usuario %d\n", op, reqID );
	
	switch( op ) {
		case EXIT: 
			return EndApp();
			break;
		case NEW_CLIENT:
			//return NewClient(reqID,data);
			break;

		default:
			fprintf(stderr,"Operacion no definida\n"); //solo para debug
			return FALSE;
	}
}*/
/*void
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
	
}*/
