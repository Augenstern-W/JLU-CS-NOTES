//RemPriv.cpp
#define SECURITY_WIN32
#include "windows.h"
#include "security.h"
#include "stdio.h"
#include "string.h"
#pragma comment(lib,"advapi32.lib")

#ifndef SE_PRIVILEGE_REMOVED
#define SE_PRIVILEGE_REMOVED (0x00000004)
#endif
DWORD RemovePrivs(LPCTSTR szPrivs[], DWORD cPrivs)
{
	//int i;
	HANDLE hProcessToken=NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hProcessToken))
		return GetLastError();
	DWORD cbBuff=sizeof(TOKEN_PRIVILEGES)+(sizeof(LUID_AND_ATTRIBUTES)*cPrivs);
	char *pbBuff=new char[cbBuff];
	PTOKEN_PRIVILEGES pTokPrivs=(PTOKEN_PRIVILEGES)pbBuff;
		//remove two privileges
	pTokPrivs->PrivilegeCount=cPrivs;
	for(DWORD i=0; i<cPrivs; i++){
		LookupPrivilegeValue(NULL, szPrivs[i], &(pTokPrivs->Privileges[i].Luid));
		pTokPrivs->Privileges[i].Attributes=SE_PRIVILEGE_REMOVED;
	}
	//remove the privileges
	BOOL fRet=AdjustTokenPrivileges(hProcessToken, FALSE, pTokPrivs, 0, NULL, NULL);
	DWORD dwErr=GetLastError();
#ifdef _DEBUG
	printf("AdjustTokenPrivileges() -> %d\nGetLastError() ->%d\n", fRet, dwErr);
#endif
	if(pbBuff) delete [] pbBuff;
	CloseHandle(hProcessToken);
	return dwErr;
}
int main(int argc, char *argv[]) {
	LPCTSTR szPrivs[]={SE_TAKE_OWNERSHIP_NAME, SE_DEBUG_NAME};
	if(RemovePrivs(szPrivs, sizeof(szPrivs)/sizeof(szPrivs[0]))==0){
		printf("Cool! It worked!\n");
	}
	return 0;
}

