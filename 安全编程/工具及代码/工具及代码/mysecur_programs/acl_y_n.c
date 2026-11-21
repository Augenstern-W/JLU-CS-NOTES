#include <stdio.h>
#include <windows.h>
void main()
{
	char *szVol="c:\\";
	DWORD dwFlags=0;
	if(GetVolumeInformation(szVol,NULL,0,NULL,NULL,&dwFlags,NULL,0))
	{
		printf("Volume %s does %s support ACLs.",szVol,
		(dwFlags & FS_PERSISTENT_ACLS) ? "":" not");
	}
	else
	{
		printf("Error %d",GetLastError());
	}
}
