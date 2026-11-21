/*checkstack.c*/

#include <stdio.h>
#include <string.h>

void foo(const char* s)
{
	char buf[28];
	strcpy(buf,s);
	return;
}

int main(int argc, char* argv[])
{
	foo(argv[1]);
	return 0;
}
