#include "Transport.h"
#include "ipcInterface.h"
#include "fileHandler.h"

int 
InitTransport(void)
{
	if( DirExists(COMM_DIR) ) {
		RemoveDir(COMM_DIR);
	}
	
    return !mkdir(COMM_DIR,0700)?OK:ERROR;
}

void
InitChildProcess(pid_t pid)
{
    InitIPC(pid);
}

