/*MyToken*/
#define SECURITY_WIN32
#include "windows.h"
#include "security.h"
#include "stdio.h"
#include "string.h"

#pragma comment(lib, "advapi32.lib")

#define MAX_NAME 256
//This function determines memory required
//and allocates it. The memory must be freed by caller.
LPVOID AllocateTokenInfoBuffer(HANDLE hToken, TOKEN_INFORMATION_CLASS InfoClass, DWORD *dwSize)
{
	*dwSize=0;
	GetTokenInformation(hToken, InfoClass, NULL, *dwSize, dwSize);
	return new BYTE[*dwSize];
}

void GetUserNames()
{
	char szName[128];
	DWORD cbName=sizeof(szName);
	if(GetUserName(szName, &cbName)) 
		printf("Name: %s\n", szName);
}
//Display SIDs and Restricting SIDs
void GetAllSIDs(HANDLE hToken, TOKEN_INFORMATION_CLASS tic)
{
	DWORD dwSize=0;
	TOKEN_GROUPS *pSIDInfo=(PTOKEN_GROUPS) AllocateTokenInfoBuffer(hToken, tic, &dwSize);
	if(!pSIDInfo) return;
	if(!GetTokenInformation(hToken, tic, pSIDInfo, dwSize, &dwSize))
		printf("GetTokenInformation Error %u\n\n", GetLastError());
	if(!pSIDInfo->GroupCount) printf("\tNone!\n");
	for(DWORD i=0; i<pSIDInfo->GroupCount; i++)
	{
		SID_NAME_USE SidType;
		char lpName[MAX_NAME];
		char lpDomain[MAX_NAME];
		DWORD dwNameSize=MAX_NAME;
		DWORD dwDomainSize=MAX_NAME;
		DWORD dwAttr=0;
		if(!LookupAccountSid(NULL, pSIDInfo->Groups[i].Sid, lpName, &dwNameSize, lpDomain, 
			&dwDomainSize, & SidType))
		{
			if(GetLastError()==ERROR_NONE_MAPPED)
				//strncpy(lpName, sizeof(lpName), "NONE_MAPPED");
				strcpy(lpName, "NONE_MAPPED");
			else printf("LookupAccountSid Error %u\n", GetLastError());
		} else dwAttr=pSIDInfo->Groups[i].Attributes;
		printf("%12s\\%-20s\t%s\n", lpDomain, lpName, 
			(dwAttr&SE_GROUP_USE_FOR_DENY_ONLY) ? "[DENY]":"");
	}
	delete [] (LPBYTE)pSIDInfo;
}
//Display privileges.
void GetPrivis(HANDLE hToken)
{
	DWORD dwSize=0;
	TOKEN_PRIVILEGES *pPrivileges=(PTOKEN_PRIVILEGES)AllocateTokenInfoBuffer(hToken, 
		TokenPrivileges, &dwSize);
	if(!pPrivileges) return;
	BOOL bRes=GetTokenInformation(hToken, TokenPrivileges, pPrivileges, dwSize, &dwSize);
	if(FALSE==bRes) printf("GetTokenInformation failed\n");
	for(DWORD i=0; i<pPrivileges->PrivilegeCount; i++)
	{
		char szPrivilegeName[128];
		DWORD dwPrivilegeNameLength=sizeof(szPrivilegeName);
		if(LookupPrivilegeName(NULL, &pPrivileges->Privileges[i].Luid, 
			szPrivilegeName, &dwPrivilegeNameLength))
			printf("\t%s (%lu)\n", szPrivilegeName, 
				pPrivileges->Privileges[i].Attributes);
		else
			printf("LookupPrivilegeName failed - %lu\n", GetLastError());
	}
	delete [] (LPBYTE)pPrivileges;
}
int main()
{
	if(!ImpersonateSelf(SecurityImpersonation))
	{
		printf("ImpersonateSelf Error %u\n", GetLastError());
		return -1;
	}
	HANDLE hToken=NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		printf("OpenThreadToken Error %u\n", GetLastError());
		return -1;
	}
	printf("\nUser Name\n");
	GetUserNames();
	printf("\nSIDS\n");
	GetAllSIDs(hToken, TokenGroups);
	printf("\nRestricting SIDS\n");
	GetAllSIDs(hToken, TokenRestrictedSids);
	printf("\nPrivileges\n");
	GetPrivis(hToken);
	RevertToSelf();
	CloseHandle(hToken);
	return 0;
}
