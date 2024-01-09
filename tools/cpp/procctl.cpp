/*
 * 程序名：procctl.cpp 本程序用于定时调用服务程序
 * 时间：2024-01-10 00:48
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("Using:./procctl timetval program argv ...\n");
    printf(
        "Example:/project/tools/bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc /logs/idc/crtsurfdata.log csv,xml,json\n\n");

    printf("本程序是服务程序的调度程序，周期性启动服务程序或shell脚本。\n");
    printf("timetvl 运行周期，单位：秒。被调度的程序运行结束后，在timetvl秒后会被procctl重新启动。\n");
    printf("program 被调度的程序名，必须使用全路径。\n");
    printf("argvs   被调度的程序的参数。\n");
    printf("注意，本程序不会被kill杀死，但可以用kill -9强行杀死。\n\n\n");
    return -1;
  }

  // 关闭信号干扰和IO输入输出
  for (int ii = 0; ii < 64; ii++)
  {
    signal(ii, SIG_IGN);
    close(ii);
  }

  // 让程序运行在后台，由系统1号进程管理
  if (fork() > 0) exit(0);
  signal(SIGCHLD, SIG_DFL);  // 恢复子进程发出SIGCHLD信号，父进程得以阻塞等待

  // 填充调度的服务程序参数
  char *pargv[argc];
  // 从第三位开始填充变量
  for (int ii = 0; ii < argc - 2; ii++) pargv[ii] = argv[ii + 2];
  pargv[argc - 2] = NULL;  // 要以(char*)0结束

  while (1)
  {
    // 子进程被替换进程映像
    if (fork() == 0)
    {
      execv(argv[2], pargv);  // 第一个参数填第三位argv[2]
      exit(0);  // 如果执行错误则退出
    }
    else
    {
      int status;
      wait(&status);  // 父进程阻塞等待子进程执行任务，当子进程执行完后按照约定时间间隔调度服务程序
      sleep(atoi(argv[1]));
    }
  }  // while

}
