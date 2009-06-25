
.SILENT:
EJECUTABLE = client
APP = App
TRANSP = Transport
LIB = Lib
SESSION = Session

CC = gcc
CC_OP = -g -c -o
LD = gcc
LD_OP = -Wall -lpthread -ldl -o

OBJS = Application.o fileHandler.o Prompt.o Server.o Transport.o ipc.o genlib.o pqADT.o inotifyMsg.o scannerADT.o tree.o Session.o CallApp.o main.o OutputPipe.o inotify.o listADT.o

help:
	@-echo ""
	@-echo ""
	@-echo "Opciones Disponibles"
	@-echo "===================="
	@-echo "make fifo = Compila usando el IPC FIFO."
	@-echo ""
	@-echo "make mq = Compila usando el IPC Message Queue."
	@-echo ""
	@-echo "make sockets = Compila usando el IPC Sockets."
	@-echo ""
	@-echo "make shmem = Compila usando el IPC Shared Memory."
	@-echo ""
	@-echo "make clean = Remueve todos los archivos objetos creados durante la compilacion."
	@-echo ""
	@-echo "make clear = Remueve todos los archivos objetos creados durante la compilacion como tambien el archivo ejecutable creado."
	@-echo ""
	@-echo ""

fifo:
	@-echo ""
	@-echo ""
	@-echo "El ejecutable es $(EJECUTABLE) y se comunica usando FIFOs."
	@-echo ""
	@-echo ""
	$(CC) $(CC_OP) Application.o $(APP)/Application.c
	$(CC) $(CC_OP) fileHandler.o $(APP)/fileHandler.c
	$(CC) $(CC_OP) inotify.o $(APP)/inotify.c
	$(CC) $(CC_OP) Prompt.o $(APP)/Prompt.c
	$(CC) $(CC_OP) Server.o $(APP)/Server.c
	$(CC) $(CC_OP) Transport.o $(TRANSP)/Transport.c
	$(CC) $(CC_OP) OutputPipe.o $(TRANSP)/OutputPipe.c
	$(CC) $(CC_OP) inotifyMsg.o $(TRANSP)/inotifyMsg.c
	$(CC) $(CC_OP) ipc.o $(TRANSP)/ipcFifo.c
	$(CC) $(CC_OP) genlib.o $(LIB)/genlib.c
	$(CC) $(CC_OP) pqADT.o $(LIB)/pqADT.c
	$(CC) $(CC_OP) listADT.o $(LIB)/listADT.c
	$(CC) $(CC_OP) scannerADT.o $(LIB)/scannerADT.c
	$(CC) $(CC_OP) tree.o $(LIB)/tree.c
	$(CC) $(CC_OP) Session.o $(SESSION)/Session.c
	$(CC) $(CC_OP) CallApp.o $(SESSION)/CallApp.c
	$(CC) $(CC_OP) main.o main.c
	$(LD) $(LD_OP) $(EJECUTABLE) $(OBJS)
	- make clean

mq:
	@-echo ""
	@-echo ""
	@-echo "El ejecutable es $(EJECUTABLE) y se comunica usando Message Queues."
	@-echo ""
	@-echo ""
	$(CC) $(CC_OP) Application.o $(APP)/Application.c
	$(CC) $(CC_OP) fileHandler.o $(APP)/fileHandler.c
	$(CC) $(CC_OP) inotify.o $(APP)/inotify.c
	$(CC) $(CC_OP) Prompt.o $(APP)/Prompt.c
	$(CC) $(CC_OP) Server.o $(APP)/Server.c
	$(CC) $(CC_OP) Transport.o $(TRANSP)/Transport.c
	$(CC) $(CC_OP) OutputPipe.o $(TRANSP)/OutputPipe.c
	$(CC) $(CC_OP) ipc.o $(TRANSP)/ipcMQ.c
	$(CC) $(CC_OP) inotifyMsg.o $(TRANSP)/inotifyMsg.c
	$(CC) $(CC_OP) genlib.o $(LIB)/genlib.c
	$(CC) $(CC_OP) pqADT.o $(LIB)/pqADT.c
	$(CC) $(CC_OP) listADT.o $(LIB)/listADT.c
	$(CC) $(CC_OP) scannerADT.o $(LIB)/scannerADT.c
	$(CC) $(CC_OP) tree.o $(LIB)/tree.c
	$(CC) $(CC_OP) Session.o $(SESSION)/Session.c
	$(CC) $(CC_OP) CallApp.o $(SESSION)/CallApp.c
	$(CC) $(CC_OP) main.o main.c
	$(LD) $(LD_OP) $(EJECUTABLE) $(OBJS)
	- make clean

sockets:
	@-echo ""
	@-echo ""
	@-echo "El ejecutable es $(EJECUTABLE) y se comunica usando Sockets."
	@-echo ""
	@-echo ""
	$(CC) $(CC_OP) Application.o $(APP)/Application.c
	$(CC) $(CC_OP) fileHandler.o $(APP)/fileHandler.c
	$(CC) $(CC_OP) inotify.o $(APP)/inotify.c
	$(CC) $(CC_OP) Prompt.o $(APP)/Prompt.c
	$(CC) $(CC_OP) Server.o $(APP)/Server.c
	$(CC) $(CC_OP) Transport.o $(TRANSP)/Transport.c
	$(CC) $(CC_OP) OutputPipe.o $(TRANSP)/OutputPipe.c
	$(CC) $(CC_OP) ipc.o $(TRANSP)/ipcSockets.c
	$(CC) $(CC_OP) inotifyMsg.o $(TRANSP)/inotifyMsg.c
	$(CC) $(CC_OP) genlib.o $(LIB)/genlib.c
	$(CC) $(CC_OP) pqADT.o $(LIB)/pqADT.c
	$(CC) $(CC_OP) listADT.o $(LIB)/listADT.c
	$(CC) $(CC_OP) scannerADT.o $(LIB)/scannerADT.c
	$(CC) $(CC_OP) tree.o $(LIB)/tree.c
	$(CC) $(CC_OP) Session.o $(SESSION)/Session.c
	$(CC) $(CC_OP) CallApp.o $(SESSION)/CallApp.c
	$(CC) $(CC_OP) main.o main.c
	$(LD) $(LD_OP) $(EJECUTABLE) $(OBJS)
	- make clean

shmem:
	@-echo ""
	@-echo ""
	@-echo "El ejecutable es $(EJECUTABLE) y se comunica usando Shared Memory."
	@-echo ""
	@-echo ""
	$(CC) $(CC_OP) Application.o $(APP)/Application.c
	$(CC) $(CC_OP) fileHandler.o $(APP)/fileHandler.c
	$(CC) $(CC_OP) inotify.o $(APP)/inotify.c
	$(CC) $(CC_OP) Prompt.o $(APP)/Prompt.c
	$(CC) $(CC_OP) Server.o $(APP)/Server.c
	$(CC) $(CC_OP) Transport.o $(TRANSP)/Transport.c
	$(CC) $(CC_OP) OutputPipe.o $(TRANSP)/OutputPipe.c
	$(CC) $(CC_OP) ipc.o $(TRANSP)/ipcSM.c
	$(CC) $(CC_OP) inotifyMsg.o $(TRANSP)/inotifyMsg.c
	$(CC) $(CC_OP) genlib.o $(LIB)/genlib.c
	$(CC) $(CC_OP) pqADT.o $(LIB)/pqADT.c
	$(CC) $(CC_OP) listADT.o $(LIB)/listADT.c
	$(CC) $(CC_OP) scannerADT.o $(LIB)/scannerADT.c
	$(CC) $(CC_OP) tree.o $(LIB)/tree.c
	$(CC) $(CC_OP) Session.o $(SESSION)/Session.c
	$(CC) $(CC_OP) CallApp.o $(SESSION)/CallApp.c
	$(CC) $(CC_OP) main.o main.c
	$(LD) $(LD_OP) $(EJECUTABLE) $(OBJS)
	- make clean


clear:
	@-echo ""
	@-echo ""
	@-echo "Limpiando el directorio"
	@-echo "Removiendo $(EJECUTABLE)"
	- rm $(EJECUTABLE)
	- make clean

clean:
	@-echo "Removiendo *.o"
	- rm *.o
	@-echo ""
	@-echo ""


