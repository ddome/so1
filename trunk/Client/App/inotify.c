#include "inotify.h"

#define EVENT_SIZE  (sizeof (struct inotify_event))
#define BUF_LEN        (1024 * (EVENT_SIZE + 16))
#define IS_DIR_MASK 0x40000000
#define PARAMS IN_DELETE | IN_DELETE_SELF | IN_CREATE | IN_MOVED_FROM | IN_MOVED_TO | IN_MODIFY



/*IN_MODIFY*/
typedef struct dirWD_T{
    int wd;
    char * path;
}dirWD_T;

typedef struct resp_T{
    char path[MAX_DIR_NAME];
    int isDir;
    int opCode;
}resp_T;

extern char * bk_path;
extern char name[MAX_LINE];

resp_T * read_events (int fd,listADT list,int * lastCookie,int* lastMask);

dirWD_T * MakeDirWD(int wd,char * path);

int NotifyServer(pid_t pid, key_t key, resp_T * resp, char name[MAX_LINE]);

int
AddNewDir(int fd, char * pathAux,listADT list)
{
    char *auxS;
    int cant,i,wd;
    dirWD_T * aux;
    char ** paths;
    
    
    /*FALTA: Agregar a la estructura de busqueda*/
    wd = inotify_add_watch (fd, pathAux,PARAMS );

    if(wd<0)
    {
	if(errno==EACCES)
	    printf("Read access to the given file is not permitted. \n");
	else if(errno==EBADF)
	    printf("The given file descriptor is not valid. . \n");
    else if(errno==EFAULT)// 
	    printf("pathname points outside of the process's accessible address space.  \n");
	else if(errno==EINVAL)
	    printf("The given event mask contains no legal events; or fd is not an inotify file descriptor.  \n");
	else if(errno==ENOMEM)
	    printf("Insufficient kernel memory was available.\n");
	else
	    printf("The user limit on the total number of inotify watches was reached or the kernel failed to allocate a needed resource.\n");
	
	return ERROR;
    }
    aux=MakeDirWD(wd,pathAux);
    printf("WD %d\n",wd);
    Insert(list,aux);
    /*free(aux);*/
    cant=DirPathList(pathAux, &paths);
    /*free(pathAux);*/
    for(i=0;i<cant;i++)
    {
	auxS=paths[i];
        AddNewDir(fd,auxS ,list);
        /*free(auxS);*/
    }
    
    /*free(paths);*/
    return OK;
}

int
Compare(dirWD_T * elem1,dirWD_T * elem2)
{
    if(elem1->wd>elem2->wd)
        return 1;
    else
    {
        if(elem1->wd<elem2->wd)
            return -1;
        else
            return 0;
    }
}

void
FreeElement(dirWD_T * elem)
{
    free(elem->path);
    free(elem);
}

dirWD_T *
MakeDirWD(int wd,char * path)
{
    dirWD_T * resp;
    if( (resp=malloc(sizeof(struct dirWD_T)))==NULL )
    {
        WritePrompt ("Error: inotify_init");
        return NULL;
    }
    resp->wd=wd;
    resp->path=CreateString(path);
    return resp;
}

int
inotifyWatcher(process_t process)
{    
    int lastCookie=0;
    int lastMask=0;
    int fd;
    int ret=0;
    int error=0;
    key_t key;
    char * pathAux, * serverPath;
    char signal = __INOTIFY_NO_DATA__;
    resp_T * resp;
    listADT list;
    list=Newlist( (int (*)(void*,void*))Compare,(void (*)(void*))FreeElement);
    if(list==NULL)
    {
        fprintf(stderr,"Error al crear la lista.\n");
        return ERROR;
    }
    
    /* Se espera el OK de que ya esta creada la carpeta
    *  a vigilar.
    */

    while(signal == __INOTIFY_NO_DATA__)
    {
      signal = ReadINotifyMsg();
    }
    
    /* Se inicia inotify. 
    */

    fd = inotify_init();
    if (fd < 0)
    {
        printf ("Error: inotify_init\n");
        return ERROR;
    }

    /*Agregar directorio y subdirectorios al vigilador.
    */
    pathAux=Concat(BK_PATH_CLIENT,process.dir);
    serverPath = Concat(bk_path, process.dir);
    key = ftok(serverPath, __DEFAULT_PID__);
    
    ret=AddNewDir(fd,pathAux,list);
    
    if(ret==ERROR)
    {
      close(fd);
      return ERROR;
    }

    while (1)
    {
        printf("Leo evento.\n");
        printf("===========\n");
        resp=read_events(fd,list,&lastCookie,&lastMask);

	    if( resp->opCode==BORRAR )
	        WritePrompt("Borrar");
	    else if( resp->opCode==CREAR )
	        WritePrompt("Crear");
	    else if( resp->opCode==MODIFICAR )
	        WritePrompt("Modificar");
	    else
	        error=1;
        
        /* Aviso al servidor de la modificacion
        */
        ret = NotifyServer(process.pid, key, resp, name);
        
	    if(!error)
	        printf("%s - %s no se directorio\n",resp->path,resp->isDir?"SI":"NO");
	    error=0;
    }
    if(pathAux != NULL)
        free(pathAux);
    FreeList(list);
    return OK;
}

int
print_mask_info ( unsigned int mask , int isDir,struct inotify_event * event,int fd,char * pathName,listADT list)
{
    printf("(%x) ", mask);
    char * pathAux;
    int ret=0;
    char * aux;
    dirWD_T * data;

    switch (mask)
    {
        case IN_ACCESS:        		printf ("File was accessed (read)\n");
        break;
        case IN_ATTRIB:           	printf ("Metadata changed\n");
        break;
        case IN_CLOSE_WRITE: 
            printf ("File opened for writing was closed\n");
        break;
        case IN_CLOSE_NOWRITE:    	printf ("File not opened for writing was closed\n");
        break;
        case IN_CREATE:        		printf ("File/directory created in watched directory\n");
					ret=IN_CREATE;
                                        if(isDir)
                                        {
                                            aux=Concat(pathName,"/");
                                            pathAux=Concat(aux,event->name);
                                            free(aux);
                                            /*printf("%s\n",pathAux);*/
                                            AddNewDir(fd,pathAux,list);
                                            free(pathAux);
                                        }
        break;
        case IN_DELETE:        		printf ("File/directory deleted from watched directory\n");
					ret=IN_DELETE;
					/*if(isDir)
					{
					    aux=Concat(pathName,"/");
                                            pathAux=Concat(aux,event->name);
                                            free(aux);
					    data=MakeDirWD(wd,pathAux);
					    Delete(list,data);
					    free(pathAux);
					}*/
        break;
        case IN_DELETE_SELF:    	printf ("Watched file/directory was itself deleted\n");
					ret=IN_DELETE_SELF;
					/*if(isDir)
					{
					    aux=Concat(pathName,"/");
                                            pathAux=Concat(aux,event->name);
                                            free(aux);
					    data=MakeDirWD(wd,pathAux);
					    Delete(list,data);
					    free(pathAux);
					}*/
        break;
        case IN_MODIFY:        		printf ("File was modified\n");
					ret=IN_MODIFY;
        break;
        case IN_MOVE_SELF:    		printf ("Watched file/directory was itself moved\n");
        break;
        case IN_MOVED_FROM:    		printf ("File moved out of watched directory\n");
					ret=IN_MOVED_FROM;
					/*if(isDir)
					{
					    aux=Concat(pathName,"/");
                                            pathAux=Concat(aux,event->name);
                                            free(aux);
					    data=MakeDirWD(wd,pathAux);
					    Delete(list,data);
					    free(pathAux);
					}*/
        break;
        case IN_MOVED_TO:    		printf ("File moved into watched directory\n");
					ret=IN_MOVED_TO;
                                        if(isDir)
                                        {
                                            aux=Concat(pathName,"/");
                                            pathAux=Concat(aux,event->name);
                                            free(aux);
                                            /*printf("%s\n",pathAux);*/
                                            AddNewDir(fd,pathAux,list);
                                            free(pathAux);
                                        }
        break;
        case IN_OPEN:        		printf ("File was opened\n");
        break;
        default:        		printf ("Error\n");
    }
    
    return ret;
}

char *
GetNewPath(struct inotify_event * event,listADT list)
{
    char * resp=NULL;
    dirWD_T * aux;
    int ret;
    int wdAux=event->wd;
    SetBegin(list);
    ret=GetData(list,(void **)&aux);
    while(ret==1 && aux->wd!=wdAux)
    {
        ret=GetData(list,(void **)&aux);
    }
    if(aux->wd==wdAux)
	resp=aux->path;
    return resp;
}

resp_T *
read_events (int fd,listADT list,int * lastCookie,int* lastMask)
{
    char buf[BUF_LEN];
    char * aux,*pathAux;

    int len, i = 0,ret;
    unsigned int mask;
    resp_T * resp=malloc(sizeof(struct resp_T));
    struct inotify_event *event;
    int isDir=0;
    if(resp==NULL)
	return NULL;
    len = read (fd, buf, BUF_LEN);
    event = (struct inotify_event *) &buf[i];
    if((event->name)[0]!='.')
    {
	if (len <= 0)
	{
	    printf ("Error al leer evento.\n");
	    return NULL;
	}
	
	while (i < len)
	{
	    printf("Proceso evento.\n");   
	    mask=event->mask;
	    if((mask&IS_DIR_MASK)==IS_DIR_MASK)
	    {
		mask=mask^IS_DIR_MASK;
		isDir=1;
	    }
	    aux=GetNewPath(event,list);
	    printf("(%s)\n",aux);
	    ret=print_mask_info ( mask , isDir,event,fd,aux,list);
	    /*free(aux);*/
	    pathAux=Concat(aux,"/");
	    pathAux=Concat(pathAux,event->name);
	    
	    printf("Last cookie %d\n",*lastCookie);
	    printf("Cookie %d\n",event->cookie);
	    printf("Last Mask %x\n",*lastMask);
	    printf("Mask %x\n",mask);
	    printf("Path entero del archivo: (%s)\n",pathAux);
	    
	    if (event->len)
	    {
		printf ("name=%s\n", event->name);
	    }
	    
	    if(*lastCookie==event->cookie && *lastMask==40 && mask==80)
	    {
		/*PARA PONER RENAME*/
	    }
	    else if( *lastCookie==event->cookie && *lastMask==40 && mask!=80)
	    {
		/*PARA PONER RENAME*/
	    }
	    else
	    {
		if(ret==IN_MODIFY)
		    resp->opCode=MODIFICAR;
		else if(ret==IN_CREATE || ret==IN_MOVED_TO)
		    resp->opCode=CREAR;
		else if(ret==IN_DELETE || ret==IN_DELETE_SELF || ret==IN_MOVED_FROM)
		    resp->opCode=BORRAR;
		else
		    resp->opCode=ERROR;
		
		resp->isDir=isDir;
		strcpy(resp->path,pathAux);
	    }
	    *lastCookie=event->cookie;
	    *lastMask=mask;
	    printf ("\n");

	    i += EVENT_SIZE + event->len;
	    isDir=0;
	}
    }
    resp->opCode=ERROR;

    return resp;
}

int
NotifyServer(pid_t pid, key_t key, resp_T * resp, char name[MAX_LINE])
{
    fileT file;
    char * path, * fileName;
    path = GetPathFromBackup(resp->path);
    fileName = GetFileName(resp->path);
    WritePrompt(path);
    WritePrompt(fileName);
    while(InitCommunication(key) == ERROR)
      usleep(__POOL_WAIT__);
    
    file = NewFileT(path, fileName);
        fopen("antesdesend", "w+");
    if(resp->opCode == BORRAR)
        return SendFileRemPack( name, file, pid );
    else
        return OK;
}






























