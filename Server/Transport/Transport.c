#include "Transport.h"
#include "TransportDefines.h"
#include "ipcInterface.h"


void
InitChildProcess(pid_t pid)
{
    InitIPC(pid);
}
