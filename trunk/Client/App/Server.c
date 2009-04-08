/* Archivo: Server.c
*  -----------------
*  Implementacion de la interface Server.h, la cual maneja la creacion,
*  continuidad y comportamiento de procesos.
*/

/*
*  Project Includes
*/

#include "Server.h"

/*
*  Functions
*/

int
StartListening(void)
{
  int status;
  byte * data;
  
  status = InitCommunication(__DEFAULT_PID__);
  if(status == ERROR)
  {
    return ERROR;
  }
  
  status = SpawnSubProcess(__SPAWN_PROMPT__, __DEFAULT_PID__, NULL); 
  if(status == CHILD_RETURN)
  {
    return OK;
  }

  while(status != ERROR && status != __SHUT_DOWN__)
  {
    data = ReadRequest();
    if(data != NULL)
    {
        /* se manda a que sea procesado en la capa de sesion 
        */
      status = ProcessRequest(&data, 0);
    }
  }
  return status;
}

byte *
ReadRequest(void)
{
  byte * data;
  int requestExists = FALSE;
  int status;
  while(!requestExists)
  {
    if( (data = GetRequest()) != NULL)
    {
      requestExists = TRUE;
    }
  }
    
  return data;
}

int
SpawnSubProcess(int opCode, pid_t pid, char msg[MAX_MSG])
{
  pid_t childPid;
  int returnValue = OK;
    
  switch(childPid = fork())
  {
    case ERROR: 
      returnValue = ERROR;
      break;
    case __ISCHILD__:
      returnValue = StartSubProcess(opCode, getppid(), msg);
      break;
        /*  Si es el padre no se hace nada
        */
    default:
      break;
  }

  return returnValue;
}


int StartSubProcess(int opCode, pid_t pid, char msg[MAX_MSG])
{
  int returnValue = OK;
  switch(opCode)
  {
    case __SPAWN_PROMPT__:
      returnValue = Prompt();
      break;
    case __SPAWN_DIR__:
      returnValue = StartDirSubServer(pid, msg);
      break;
    /*case __SPAWN_INOTIFY__:
      returnValue = */
    case __SPAWN_PING__:
      returnValue = StartPingServer(pid, msg);
      break;
    case __SPAWN_DEMAND__:
      returnValue = StartDemandSubServer(pid, msg);
      break;
    /* Si no era un codigo de operacion valido, se devuelve error
    */
    default:
      returnValue = ERROR;
      break;
  }
    
  return returnValue;
}

int StartDirSubServer(pid_t pid, char msg[MAX_MSG])
{
  int status;
  key_t key;
  byte * data;
  key = ftok(msg, pid);
  if(InitCommunication(key) == ERROR)
  {
    status = ERROR;
  }

  while(status != ERROR && status != __SHUT_DOWN__)
  {
    data = ReadDirSubServerRequests();
    if(data != NULL)
    {
        /* se manda a que sea procesado en la capa de sesion 
        */
        status = ProcessRequest(&data, 0);
    }
    else
    {
      status = ERROR;
    }
  }

  return status;
}

byte * ReadDirSubServerRequests(void)
{
  byte * data;
  int requestExists = FALSE;
  int status;
  while(!requestExists)
  {
    if( (data = GetRequest()) != NULL)
    {
      requestExists = TRUE;
    }
  }
    
  return data;
}

int StartDemandSubServer(pid_t pid, char msg[MAX_MSG])
{
  return 1;
}

int StartPingServer(pid_t pid, char msg[MAX_MSG])
{
    return 1;
}
