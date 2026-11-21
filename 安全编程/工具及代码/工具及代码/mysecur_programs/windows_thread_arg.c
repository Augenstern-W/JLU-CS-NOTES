#include <stdio.h>
#include <windows.h>
#include <process.h>

int a = 0; //globle var

void fun2(void *pArg){
    printf("Value from main thread:%d\n\n", *(int *)pArg);
    a = *(int *)pArg;
    while(1){
        a++;
        if(a>=10) break;
        Sleep(200);
        printf("a = %d, new thread running...\n", a);
    } 
    printf("new thread finished!\n\n");
    _endthread();
}

int main(){
    int b = 5;
    int* arg = &b;
    _beginthread(fun2, 0, (void *)arg);
    while(1){
        if(a >= 10){
            printf("main thread finished\n");
            break;
        }
    }
    return 0;
}
