/*Restrict_Thread_Token.cpp*/
#include <windows.h>
DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
	DWORD dwErr=0;
	try {
		if(!ImpersonateSelf(SecurityImpersonation)) throw GetLastError();
		HANDLE hToken=NULL;
		HANDLE hThread=GetCurrentThread();
		if(!OpenThreadToken(hThread,
			 TOKEN_ASSIGN_PRIMARY|TOKEN_DUPLICATE|TOKEN_QUERY|TOKEN_IMPERSONATE, 
			TRUE, &hToken)) throw GetLastError();
		HANDLE hNewToken=NULL;
		if(!CreateRestrictedToken(hToken, DISABLE_MAX_PRIVILEGE, 0, NULL, 0, NULL, 0, NULL, 
			&hNewToken)) throw GetLastError();
		if(!SetThreadToken(&hThread, hNewToken)) throw GetLastError();
		//DoThreadWork operates in restricted context.
		DoThreadWork(hNewToken);
	} catch (DWORD d)
	{
		dwErr=d;
	}
	if(dwErr==0) RevertToSelf();
	return dwErr;
}
void main()
{
	HANDLE h=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, NULL, CREATE_SUSPENDED, NULL);
	if(h) ResumeThread(h);
}
