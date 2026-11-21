
#include <windows.h>
#include <sddl.h>
#include <stdio.h>
#pragma comment(lib,"advapi32.lib")


BOOL CreateMyDACL(SECURITY_ATTRIBUTES *);

int main()
{
     SECURITY_ATTRIBUTES  sa;
      
     sa.nLength = sizeof(SECURITY_ATTRIBUTES);
     sa.bInheritHandle = FALSE;  

     if (FALSE==CreateMyDACL(&sa))
     {
         printf("Failed CreateMyDACL\n");
         exit(1);
     }

     if (0 == CreateDirectory("E:\\MyFolder", &sa))
     {
         printf("Failed CreateDirectory\n");
         exit(1);
     }

     if (NULL != LocalFree(sa.lpSecurityDescriptor))
     {
         printf("Failed LocalFree\n");
         exit(1);
     }
}

BOOL CreateMyDACL(SECURITY_ATTRIBUTES * pSA)
{
    char szSD[] = "D:P"        // Discretionary ACL
        "(D;OICI;GA;;;BG)"     // Deny access to 
                                     // built-in guests
        "(D;OICI;GA;;;AN)"     // Deny access to 
                                     // anonymous logon
        "(A;OICI;GRGWGX;;;AU)" // Allow 
                                     // read/write/execute 
                                     // to authenticated 
                                     // users
        "(A;OICI;GA;;;BA)";    // Allow full control 
                                     // to administrators

    if (NULL == pSA)
	{
		printf("pSA is NULL!\n");
        return FALSE;
	}

	if(!ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, ACL_REVISION1, 
		&(pSA->lpSecurityDescriptor), NULL))
	{
		printf("ConvertStringSidToSid-err:%d\n",GetLastError());
		return FALSE;
	}

	return TRUE;
}
