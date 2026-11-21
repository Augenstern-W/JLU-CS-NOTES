/*NT4ACL.CPP*/
#include <windows.h>
#include <stdio.h>
#include <aclapi.h>
PSID pEveryoneSID=NULL, pAdminSID=NULL, 
	 pNetworkSID=NULL;
PACL pACL=NULL;
PSECURITY_DESCRIPTOR pSD=NULL;

//ACL will contain three ACEs:
//Network (Deny Access)
//Everyone (Read)
//Admin (Full Control)
int main()
{
	const int NUM_ACES=3;
	EXPLICIT_ACCESS ea[NUM_ACES];
	ZeroMemory(&ea, NUM_ACES * sizeof(EXPLICIT_ACCESS));

	//Create a well-known SID for the Network logon group.
	SID_IDENTIFIER_AUTHORITY SIDAuthNT=SECURITY_NT_AUTHORITY;
	if(!AllocateAndInitializeSid(&SIDAuthNT, 1, SECURITY_NETWORK_RID, 
		0, 0, 0, 0, 0, 0, 0, &pNetworkSID))
	{
		printf("AllocateAndInitializeSid Network-err:%d\n",GetLastError());
		return -1;
	}
	ea[0].grfAccessPermissions=GENERIC_ALL;
	ea[0].grfAccessMode=DENY_ACCESS;
	ea[0].grfInheritance=NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm=TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType=TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName=(LPTSTR)pNetworkSID;
	
	//Create a well-known SID for the Everyone group.
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld=SECURITY_WORLD_SID_AUTHORITY;
	if(!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 
		0, 0, 0, 0, 0, 0, 0, &pEveryoneSID))
	{
		printf("AllocateAndInitializeSid Everyone-err:%d\n",GetLastError());
		return -1;
	}
	ea[1].grfAccessPermissions=GENERIC_READ;
	ea[1].grfAccessMode=SET_ACCESS;
	ea[1].grfInheritance=NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm=TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType=TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[1].Trustee.ptstrName=(LPTSTR)pEveryoneSID;

	//Create a SID for the BUILTIN/Administrators group.
	if(!AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,0, 0, 0, 0, 0, 0, &pAdminSID))
	{
		printf("AllocateAndInitializeSid Administrators-err:%d\n",GetLastError());
		return -1;
	}
	ea[2].grfAccessPermissions=GENERIC_ALL;
	ea[2].grfAccessMode=SET_ACCESS;
	ea[2].grfInheritance=NO_INHERITANCE;
	ea[2].Trustee.TrusteeForm=TRUSTEE_IS_SID;
	ea[2].Trustee.TrusteeType=TRUSTEE_IS_GROUP;
	ea[2].Trustee.ptstrName=(LPTSTR)pAdminSID;
	//Create a new ACL with the three ACEs.
	if(ERROR_SUCCESS !=SetEntriesInAcl(NUM_ACES, ea, NULL, &pACL))
	{
		printf("SetEntriesInAcl-err:%d\n",GetLastError());
		return -1;
	}	

	//Initialize a security descriptor.
	pSD=(PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if(pSD==NULL) 
	{
		printf("LocalAlloc for pSD-err:%d\n",GetLastError());
		return -1;	
	}

	if(!InitializeSecurityDescriptor(pSD,SECURITY_DESCRIPTOR_REVISION))
	{
		printf("InitializeSecurityDescriptor-err:%d\n",GetLastError());
		return -1;	
	}

	//Add the ACL to the security descriptor.
	if(!SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE))
	{
		printf("SetSecurityDescriptorDacl-err:%d\n",GetLastError());
		return -1;	
	}
	else
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength=sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle=FALSE;
		sa.lpSecurityDescriptor=pSD;
		if(!CreateDirectory("E:\\mydir",&sa))
		{
			printf("CreateDirectory-err:%d\n",GetLastError());
			return -1;
		}
	}
	if(pSD)	LocalFree(pSD);
	if(pACL)	LocalFree(pACL);

	//Call FreeSid for each SID allocated by AllocateAndInitializeSID.
	if(pEveryoneSID) 	FreeSid(pEveryoneSID);
	if(pNetworkSID) 	FreeSid(pNetworkSID);
	if(pAdminSID) 	FreeSid(pAdminSID);
	return 0;
}

