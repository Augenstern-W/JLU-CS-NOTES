/*SetUpdateACL.cpp*/
#include <iostream>
#include <windows.h>
#include <aclapi.h>
#include <sddl.h>
#include <stdio.h>
#include <string.h>
#include <security.h>

#pragma comment(lib, "advapi32.lib")


int main(int argc, char *argv[])
{
	char szName[]="E:\\MyFolder";
	PACL pDacl=NULL;
	PACL pNewDacl=NULL;
	PSECURITY_DESCRIPTOR sd=NULL;
	PSID sidAuthUsers=NULL;
	//DWORD cbSid;
	DWORD dwErr=0;
	dwErr=
		GetNamedSecurityInfo(
		szName,
		SE_FILE_OBJECT,
		DACL_SECURITY_INFORMATION,
		NULL,
		NULL,
		&pDacl,
		NULL,
		&sd);
		if(dwErr!=ERROR_SUCCESS)
		{
			printf("GetNamedSecurityInfo-err:%d\n",GetLastError());
			return dwErr;
		}
		
		EXPLICIT_ACCESS ea;
		ZeroMemory(&ea,sizeof(EXPLICIT_ACCESS));

		//SECURITY_CREATOR_SID_AUTHORITY
		SID_IDENTIFIER_AUTHORITY SIDnew=SECURITY_CREATOR_SID_AUTHORITY;
		if(!AllocateAndInitializeSid(&SIDnew, 1, SECURITY_CREATOR_OWNER_RID, 
			0, 0, 0, 0, 0, 0, 0, &sidAuthUsers))
		{
			printf("AllocateAndInitializeSid Network-err:%d\n",GetLastError());
			return -1;
		}
		
		char SidName[]="CO";
		PSID *newpSID;
		dwErr=ConvertStringSidToSid(SidName, newpSID);
		if(dwErr==ERROR_INVALID_PARAMETER)
		{
			printf("ConvertStringSidToSid-err:ERROR_INVALID_PARAMETER\n");
			return -1;	
		}
		else if(dwErr==ERROR_INVALID_SID)
		{
			printf("ConvertStringSidToSid-err:ERROR_INVALID_SID\n");
			return -1;	
		}
		
		BuildTrusteeWithSid(&ea.Trustee, *newpSID);

		ea.grfAccessPermissions=GENERIC_ALL;
		ea.grfAccessMode=SET_ACCESS;
		ea.grfInheritance=NO_INHERITANCE;
		ea.Trustee.TrusteeForm=TRUSTEE_IS_SID;
		ea.Trustee.TrusteeType=TRUSTEE_IS_GROUP;

		dwErr=SetEntriesInAcl(1, &ea, pDacl, &pNewDacl);
		if(dwErr!=ERROR_SUCCESS)
		{
			printf("SetEntriesInAcl-err%d\n", GetLastError());
			return dwErr;	
		}
		dwErr=SetNamedSecurityInfo(szName,
			SE_FILE_OBJECT,
			DACL_SECURITY_INFORMATION,
			NULL,
			NULL,
			pNewDacl,
			NULL);
		if(dwErr!=ERROR_SUCCESS)
		{
			printf("SetNamedSecurityInfo-err%d\n", GetLastError());
			return dwErr;	
		}
	if(sidAuthUsers)
		LocalFree(sidAuthUsers);
	if(sd)
		LocalFree(sd);
	if(pNewDacl)
		LocalFree(pNewDacl);
	return dwErr;
}	