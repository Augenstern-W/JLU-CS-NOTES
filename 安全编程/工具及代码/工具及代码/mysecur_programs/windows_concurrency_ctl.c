	/*
	 * windows_concurrency_ctl.c
	 */
	#include <windows.h>
	#include <stdio.h>
	#include <process.h>
	//global vars..
	CRITICAL_SECTION lock;
	int A = 200;
	int	B = 100;
	int C = 50;
	DWORD WINAPI B_to_C (LPVOID arg)
	{
			EnterCriticalSection(&lock);
			int b=B;
			int c=C;
			//Sleep(100);
			B=b-50;
			C=c+50;
			//Sleep(100);
			printf("Finished B to C!\n");
			LeaveCriticalSection(&lock);
			//Sleep(1);
	}
	int
	main(void)
	{
		HANDLE h = NULL;
		int ret = 0;
		int i=0;
		InitializeCriticalSection(&lock);
		h     = CreateThread(NULL, 0, B_to_C, NULL, 0, NULL);
		if(h == NULL)
		{
			printf("CreateThread() failed.\r\n");
			return(1);
		}

			EnterCriticalSection(&lock);
			int a=A;
			int b=B;
			A=a-100;
			//Sleep(1000);
			B=b+100;
			printf("Finished A to B!\n");

			LeaveCriticalSection(&lock);
			//Sleep(1000);
			//printf("A = %d; B = %d; C = %d;\n",A,B,C);
		WaitForSingleObject(h, INFINITE);
		printf("A = %d; B = %d; C = %d;\n",A,B,C);
		DeleteCriticalSection(&lock);
		return(0);
	}