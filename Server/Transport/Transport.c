#include "Transport.h"
#include "TransportDefines.h"
#include "ipcInterface.h"

int
InitTransport(void)
{
    return InitMainIPC();
}

void
InitChildProcess(pid_t pid)
{
    InitIPC(pid);
}
