#include "Transport.h"
#include "ipcInterface.h"

int 
InitTransport(void)
{
    return !mkdir(COMM_DIR,0700)?OK:ERROR;
}

void
InitChildProcess(pid_t pid)
{
    InitIPC(pid);
}

