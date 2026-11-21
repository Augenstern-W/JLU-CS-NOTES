	/*
	 * pthread1.c 
	 */	
	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
	void *thread_entry_point (void *arg)
	{
		printf("thread 2!\n");
	}	
	int
	main(void)
	{
		pthread_t 	pt;
		int	ret = 0;	
		ret = pthread_create(&pt, NULL, thread_entry_point, NULL);
		if(ret != 0x00)
		{
			printf("pthread_create() failed.\n");
			return(1);
		}
		sleep(1);
		printf("thread 1!\n");
		return(0);
	}