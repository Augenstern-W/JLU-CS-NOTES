/*ovf3.c*/

#include <stdio.h>
#include <string.h>
int *PSP;
int j,k;
void bar();
void foo();

void foo(const char* input)
{
	char buf[10];
	k=(int)&input-(int)buf;
	printf("%d\n", k);
	printf("My stack looks like:\n");
	for(j=0;j<=k;j=j+4){
		PSP=(int *)(buf+j);
		printf("%p\n",*PSP);
	}
	printf("\n");
	
	strcpy(buf,input);

	for(j=0;j<=k;j=j+4){
		PSP=(int *)(buf+j);
		printf("%p\n",*PSP);
	}
	printf("\n");
}
void bar(void)
{
	printf("Augh! I've been hacked!\n");
}
int main(int argc, char* argv[])
{
	printf("Address of foo=%p\n",foo);
	printf("Address of bar=%p\n",bar);
	if(argc!=2)
	{
		printf("Please supply a string as an argument!\n");
		return -1;
	}
	foo(argv[1]);
	return 0;
}
