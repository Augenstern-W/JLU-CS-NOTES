/*
StackOverFlow1.c
本程序总是溢出，并显示溢出前后的堆栈变化
*/
#include <stdio.h>
#include <string.h>
int PSP;
int k;
void bar();
void foo();
int addr[4];
int go;
void foo(const char* input)
{
	char buf[12];
	k=(int)&input-(int)buf;
	printf("%d\n", k);
	printf("My stack looks like:\n");
	PSP=(int)buf[0];printf("%p\n",PSP);
	PSP=(int)buf[4];printf("%p\n",PSP);
	PSP=(int)buf[8];printf("%p\n",PSP);
	PSP=(int)buf[12];printf("%p\n",PSP);
	PSP=(int)buf[16];printf("%p\n",PSP);
	PSP=(int)buf[20];printf("%p\n",PSP);
	PSP=(int)buf[24];printf("%p\n",PSP);
	PSP=(int)buf[28];printf("%p\n",PSP);
	PSP=(int)buf[32];printf("%p\n\n",PSP);	
	strcpy(buf,input);

	printf("%s\n",buf);
	
	PSP=(int)buf[0];printf("%p\n",PSP);
	PSP=(int)buf[4];printf("%p\n",PSP);
	PSP=(int)buf[8];printf("%p\n",PSP);
	PSP=(int)buf[12];printf("%p\n",PSP);
	PSP=(int)buf[16];printf("%p\n",PSP);
	PSP=(int)buf[20];printf("%p\n",PSP);
	PSP=(int)buf[24];printf("%p\n",PSP);
	PSP=(int)buf[28];printf("%p\n",PSP);
	PSP=(int)buf[32];printf("%p\n\n",PSP);
	
	printf("%s\n",buf);
	
    go=(int)&bar;
    //由于EIP地址是倒着表示的，所以首先把bar()函数的地址分离成字节
    addr[0]=(go << 24)>>24; 
	addr[1]=(go << 16)>>24; 
	addr[2]=(go << 8)>>24;  
	addr[3]=go>>24;
    for(int j=0;j<4;j++){
        buf[k-j-1]=addr[3-j];
    }
	
	PSP=(int)buf[0];printf("%p\n",PSP);
	PSP=(int)buf[4];printf("%p\n",PSP);
	PSP=(int)buf[8];printf("%p\n",PSP);
	PSP=(int)buf[12];printf("%p\n",PSP);
	PSP=(int)buf[16];printf("%p\n",PSP);
	PSP=(int)buf[20];printf("%p\n",PSP);
	PSP=(int)buf[24];printf("%p\n",PSP);
	PSP=(int)buf[28];printf("%p\n",PSP);
	PSP=(int)buf[32];printf("%p\n\n",PSP);
	
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
