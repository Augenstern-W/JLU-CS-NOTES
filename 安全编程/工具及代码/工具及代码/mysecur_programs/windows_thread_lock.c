	/*
	 * windows_thread_lock.c
	 */
	#include <windows.h>
	#include <stdio.h>
	#include <process.h>
	//global vars..
	CRITICAL_SECTION lock;
	int g_val = 0;	
	DWORD WINAPI thread_entry_point (LPVOID arg)
	{
		int i=0;
		while(g_val<20000)
		{
			EnterCriticalSection(&lock);
			++g_val;
			++i;
			printf("thread 2 , g_val:%d  i:%d\n", g_val,i);
			LeaveCriticalSection(&lock);
			Sleep(1);
		}
	}
	int
	main(void)
	{
		HANDLE h   = NULL;
		int    ret = 0;
		int i=0;
		InitializeCriticalSection(&lock);
		h     = CreateThread(NULL, 0, thread_entry_point, NULL, 0, NULL);
		if(h == NULL)
		{
			printf("CreateThread() failed.\r\n");
			return(1);
		}
		while(g_val<20000)
		{
			EnterCriticalSection(&lock);
			++g_val;
			++i;
			printf("thread 1 , g_val:%d  j:%d\n", g_val,i);
			LeaveCriticalSection(&lock);
			Sleep(1);
		}
		//WaitForSingleObject(h, INFINITE);
		//DeleteCriticalSection(&lock);
		return(0);
	}