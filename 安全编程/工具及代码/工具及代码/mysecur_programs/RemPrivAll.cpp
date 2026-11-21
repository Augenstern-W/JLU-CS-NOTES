/*RemPrivAll.cpp*/
#define SECURITY_WIN32
#include "windows.h"
#include "security.h"
#include "stdio.h"
#include "string.h"
#pragma comment(lib,"advapi32.lib")

#ifndef SE_PRIVILEGE_REMOVED
#define SE_PRIVILEGE_REMOVED (0x00000004)
#endif
#ifndef SE_PRIVILEGE_REMOVED
#define SE_PRIVILEGE_REMOVED (0x00000004)
#endif
#define SAME_LIUD(Luid1,Luid2) (Luid1.LowPart==Luid2.LowPart && Luid1.HighPart==Luid2.HighPart)
DWORD JettisonPrivs()
{
	DWORD dwError=0;
	void *TokenInfo=NULL;
	try{
		HANDLE hToken=NULL;
		if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &hToken))
			throw GetLastError();
		DWORD	dwSize=0;
		if(!GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &dwSize))
		{
			dwError=GetLastError();
			if(dwError!=ERROR_INSUFFICIENT_BUFFER) throw dwError;
		}
		TokenInfo=new char[dwSize];
		if(NULL==TokenInfo) throw ERROR_NOT_ENOUGH_MEMORY;
		if(!GetTokenInformation(hToken, TokenPrivileges, TokenInfo, dwSize, &dwSize))
			throw GetLastError();

		TOKEN_PRIVILEGES *pTokenPrivs=(TOKEN_PRIVILEGES *)TokenInfo;
				//don’t remove this privilege
		LUID luidChangeNotify;
		LookupPrivilegeValue(NULL, SE_CHANGE_NOTIFY_NAME,
			 &luidChangeNotify);
		for(DWORD dwIndex=0; dwIndex<pTokenPrivs->PrivilegeCount; 
			dwIndex++)
			if(!SAME_LIUD(pTokenPrivs->Privileges[dwIndex].Luid,
				 luidChangeNotify))
			pTokenPrivs->Privileges[dwIndex].Attributes=
				SE_PRIVILEGE_REMOVED;
		if(!AdjustTokenPrivileges(hToken, FALSE, pTokenPrivs,
			 dwSize, NULL, NULL)) throw GetLastError();
	} catch (DWORD err) {
		dwError=err;
	}
	if(TokenInfo) delete [] TokenInfo;
	return dwError;
}

int main()
{
	JettisonPrivs();
}
