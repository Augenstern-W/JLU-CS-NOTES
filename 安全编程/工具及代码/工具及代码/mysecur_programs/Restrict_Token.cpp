/*Restrict_Token.cpp*/
#define SECURITY_WIN32
#include "windows.h"
#include "security.h"
#include "stdio.h"
#include "string.h"
#pragma comment(lib,"advapi32.lib")

int main()
{

	//Create a SID for the BUILTIN\Administrators Group.
	BYTE sidBuffer[256];
	PSID pAdminSID=(PSID)sidBuffer;
	SID_IDENTIFIER_AUTHORITY SIDAuth=SECURITY_NT_AUTHORITY;
	if(!AllocateAndInitializeSid(&SIDAuth, 2, SECURITY_BUILTIN_DOMAIN_RID, 
		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,&pAdminSID))
	{
		printf("AllocateAndInitializeSid Error %u\n", GetLastError());
		return -1;
	}

	//Change the local administrator’s SID to a deny-only SID.
	SID_AND_ATTRIBUTES SidToDisable[1];
	SidToDisable[0].Sid=pAdminSID;
	SidToDisable[0].Attributes=0;
	//Get the current process token.
	HANDLE hOldToken=NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ASSIGN_PRIMARY|TOKEN_DUPLICATE|TOKEN_QUERY|TOKEN_ADJUST_DEFAULT, &hOldToken))
	{
		printf("OpenProcessToken Error %u\n", GetLastError());
		return -1;
	}

	//Create restricted token from the process token.
	HANDLE hNewToken=NULL;
	if(!CreateRestrictedToken(hOldToken, DISABLE_MAX_PRIVILEGE, 1, 
		SidToDisable, 0, NULL, 0, NULL, &hNewToken))
	{
		printf("CreateRestrictedToken Error %u\n", GetLastError());
		return -1;
	}
	if(pAdminSID) free(pAdminSID);

	//The following code creates a new process with the restricted token.
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb=sizeof(STARTUPINFO);
	si.lpDesktop=NULL;

	//Build the path to cmd.exe to make sure we’re not running a Trojaned cmd.exe.
	char szSysDir[MAX_PATH+1];
	if(GetSystemDirectory(szSysDir, MAX_PATH))
	{
		char szCmd[MAX_PATH+1];
		strncpy(szCmd,szSysDir,sizeof(szSysDir));
		strcat(szCmd,"\\");
		strcat(szCmd,"cmd.exe");
		if(!CreateProcessAsUser(hNewToken, szCmd, NULL, NULL, NULL, FALSE, 
			CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
			printf("CreateProcessAsUser Error %u\n", GetLastError());
		CloseHandle(hOldToken);
		CloseHandle(hNewToken);
		return 0;
	}
}

