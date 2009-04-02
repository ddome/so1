/* Functions */


int
InitCommunication(pid_t pid)
{
    return InitIPC(pid);
}

pid_t GetRequest(void * data)
{
    return ReadIPC(data);
}

int
ProcessRequest(byte ** data, pid_t requestPid)
{
	session_t pack;
	int ret;
	
		
	pack  = GetSessionData(*data);
    ret = ProcessCall( &pack );
	
	free(*data);
	*data = MakeSessionData(pack);

	/*if(((cacaT*)data)->dni>0)
	{
	    printf("llego un mensaje!: %d - %s es gay \n", ((cacaT*)data)->dni,((cacaT*)data)->esgay?"SI":"NO");
	    SpawnSubProcess(__SPAWN_DIR__,requestPid,"/");
	}
	else
	    printf("Rock and roll neneeee!\n");*/
	
	
	return ret;
}
