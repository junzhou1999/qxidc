/**
 * 程序名：book01.cpp   日期：2024年04月16日
 * 本程序用system()函数创建新的进程映像来调用外部cat命令读取文件内容
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

int main(int argc, char* argv[])
{
  pid_t pid = fork();

  if (pid == 0)
  {
    // string strCmd;  strCmd.clear();   strCmd.resize(1024);
    char strCmd[1024];  memset(strCmd, 0, sizeof(strCmd));
    snprintf(strCmd, 1023, "/usr/bin/cat %s.cpp", argv[0]);
    system(strCmd);     // 或者execv函数族
    exit(0);
  }
  else
  {
    // 父进程阻塞等待，避免子进程成为僵尸进程
    int status;
    wait(&status);
  }

  return 0;
}
