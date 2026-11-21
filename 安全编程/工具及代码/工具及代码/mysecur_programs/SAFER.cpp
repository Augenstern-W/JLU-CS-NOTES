/*SAFER.cpp*/
#include <windows.h>
//#include <WinSafer.h>
#include <Safer.h>
#include <winnt.h>
#include <stdio.h>

void main()
{
	SAFER_LEVEL_HANDLE hAuthzLevel;
	//Valid programmatic SAFER levels:
	//SAFER_LEVELID_FULLYTRUSTED
	//SAFER_LEVELID_NORMALUSER
	//SAFER_LEVELID_CONSTRAINED
	//SAFER_LEVELID_UNTRUSTED
	//SAFER_LEVELID_DISALLOWED
	//Create a normal user level.
	if(SaferCreateLevel(SAFER_SCOPEID_USER, SAFER_LEVELID_NORMALUSER, 0, &hAuthzLevel, NULL))
	{
		//Generate the restricted token that we will use.
		HANDLE hToken=NULL;
		if(SaferComputeTokenFromLevel(
			hAuthzLevel,		//Safer Level handle
			NULL,		//NULL is current thread token.
			&hToken,		//Target token
			0,		//No flags
			NULL))		//Reserved
		{
			//Build the path to cmd.exe to take sure
			//we are not running a Trojaned cmd.exe.
			char szPath[MAX_PATH+1], szSysDir[MAX_PATH+1];
			if(GetSystemDirectory(szSysDir, sizeof(szSysDir)))
			{
				strncpy(szPath,szSysDir,sizeof(szSysDir));
				strcat(szPath,"\\");
				strcat(szPath,"cmd.exe");
				//StringCbPrintf(szPath, sizeof(szPath), "%s\\cmd.exe", szSysDir);
				STARTUPINFO si;
				ZeroMemory(&si, sizeof(STARTUPINFO));
				si.cb=sizeof(STARTUPINFO);
				si.lpDesktop=NULL;
				PROCESS_INFOMATION pi;
				if(!CreateProcessAsUser(
					hToken,
					szPath, NULL, 
					NULL, NULL,
					FALSE, CREATE_NEW_CONSOLE, 
					NULL, NULL, 
					&si, &pi))
					printf("CreateProcessAsUser failed (%lu)\n",
						GetLastError());
			}
		}
		SaferCloseLevel(hAuthzLevel);
	}
}
