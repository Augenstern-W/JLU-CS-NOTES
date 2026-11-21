#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define MSGSIZE 16
char *msg1 = "hello, book #1";
char *msg2 = "hello, book #2";
char *msg3 = "hello, book #3";
int main()
{
	char inbuf[MSGSIZE];
	int p[2],j,pid;
	if(pipe(p) == -1)
	{
		perror("pipe call failed");
		exit(1);
	}
	switch(pid=fork()){ 
		case -1:
			perror("fork call failed");
			exit(2);
		case 0:
			/* if child then write to  pipe*/
			write(p[1], msg1, MSGSIZE);
			write(p[1], msg2, MSGSIZE);
			write(p[1], msg3, MSGSIZE);
		default:
			/*if parent then read from pipe*/
			for(j=0; j<3; j++)
			{
				read(p[0], inbuf, MSGSIZE);
				printf("%s\n", inbuf);
			}
			wait(NULL);
			exit(0);			
	}
	return 0;
} 