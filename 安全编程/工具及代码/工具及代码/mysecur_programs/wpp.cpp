
	
#include <Windows.h>
 
#include <iostream>
 
char commandline[]="c:\\TDM-GCC-32\\mysecur_programs\\vof2.exe";
int main(){
 
     std::cout << "Welcome to run father process." << std::endl;
 
     // 设置新进程的窗口信息
 
     STARTUPINFO si;
 
     memset(&si, 0, sizeof(si));
 
     si.cb = sizeof(si);
 
     // 接收新进程的标识信息
 
     PROCESS_INFORMATION pi;
 
     memset(&pi, 0, sizeof(pi));
 
 
 
     // 创建子进程
 
     BOOL ret = CreateProcess(
 
     commandline,  // 子进程名称（全路径，含扩展名）
 
     NULL,                       // 命令行参数
 
     NULL,                       // 不继承父进程的进程句柄
 
     NULL,                       // 不进程父进程的线程句柄
 
     FALSE,                      // 句柄继承标识为FALSE
 
     // CREATE_NO_WINDOW   -> 新进程，无窗口运行
 
     // CREATE_NEW_CONSOLE -> 新进程，在独立控制台窗口运行。   
 
     CREATE_NEW_CONSOLE, 
 
     NULL,                       // 使用父进程的环境
 
     NULL,                       // 使用父进程的当前目录
 
     &si,                          // 指向STARTUPINFO
 
     &pi);                        // 指向PROCESS_INFORMATION
 
 
 
     if(!ret){
 
          std::cout << "create process fail!" << std::endl;
 
          return -1;
 
     }
 
     std::cout << "create process success!" << std::endl;
 
     // 等待子进程结束
 
     WaitForSingleObject(pi.hProcess, INFINITE);
 
     std::cout << "child process finish" << std::endl;
 
     // 关闭子进程的进程句柄和线程句柄
 
     CloseHandle(pi.hProcess);
 
     CloseHandle(pi.hThread);
 
     std::cout << "father process game over!" << std::endl;
 
     return 0;

}
