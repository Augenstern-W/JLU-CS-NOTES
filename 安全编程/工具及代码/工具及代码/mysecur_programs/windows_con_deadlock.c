	/*
	 * windows_con_deadlock.c
	 */
	#include <windows.h>
	#include <stdio.h>
	#include <process.h>
	//global vars..
	CRITICAL_SECTION lockA,lockB,lockC;
	int A = 200;
	int	B = 100;
	int C = 50;
	DWORD WINAPI B_to_C (LPVOID arg)
	{
			EnterCriticalSection(&lockB);
			int b=B;
			Sleep(100);
			printf("B_to_C: got B, request C!\n");
			EnterCriticalSection(&lockC);
			int c=C;
			B=b-50;
			LeaveCriticalSection(&lockB);
			C=c+50;
			LeaveCriticalSection(&lockC);
			printf("Finished B to C!\n");
	}

	DWORD WINAPI C_to_A (LPVOID arg)
	{
			EnterCriticalSection(&lockC);
			int c=C;
			Sleep(100);
			printf("C_to_A: got C, request A!\n");
			EnterCriticalSection(&lockA);
			int a=A;
			C=c-50;
			LeaveCriticalSection(&lockC);
			A=a+50;
			LeaveCriticalSection(&lockA);
			printf("Finished C to A!\n");
	}
	
	int
	main(void)
	{
		HANDLE h1 = NULL, h2 = NULL;
		int ret = 0;
		int i=0;
		InitializeCriticalSection(&lockA);
		InitializeCriticalSection(&lockB);
		InitializeCriticalSection(&lockC);
		h1 = CreateThread(NULL, 0, B_to_C, NULL, 0, NULL);
		if(h1 == NULL)
		{
			printf("CreateThread() failed.\r\n");
			return(1);
		}
		h2 = CreateThread(NULL, 0, C_to_A, NULL, 0, NULL);
		if(h2 == NULL)
		{
			printf("CreateThread() failed.\r\n");
			return(1);
		}
			//EnterCriticalSection(&lock);
			EnterCriticalSection(&lockA);
			int a=A;
			Sleep(100);
			printf("A_to_B: got A, request B!\n");
			EnterCriticalSection(&lockB);
			int b=B;
			A=a-100;
			LeaveCriticalSection(&lockA);
			B=b+100;
			LeaveCriticalSection(&lockB);

			printf("Finished A to B!\n");
			//printf("thread 1 , g_val:%d  j:%d\n", g_val,i);
			//LeaveCriticalSection(&lock);
			//Sleep(1000);
			//printf("A = %d; B = %d; C = %d;\n",A,B,C);
		//DeleteCriticalSection(&lock);
		WaitForSingleObject(h1, INFINITE);
		WaitForSingleObject(h2, INFINITE);
		printf("A = %d; B = %d; C = %d;\n",A,B,C);
		DeleteCriticalSection(&lockA);
		DeleteCriticalSection(&lockB);
		DeleteCriticalSection(&lockC);
		return(0);
	}