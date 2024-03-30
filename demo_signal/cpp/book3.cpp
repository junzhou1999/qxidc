/*
 * 程序名：book3.cpp 本程序用于测试守护进程的demo程序
 * 时间：2024-01-10 16:48
*/

#include "_public.h"

// 2和15的处理函数
void EXIT(int sig)
{
  printf("收到信号%d，程序退出。\n", sig);
  exit(0);  // exit函数会析构全局的对象
}

// 心跳机制
CPActive pActive;

int main(int argc, char *argv[])
{
  // book3 procname timeout
  if (argc != 3)
  {
    printf("Using:./book3 procname timeout\n");
    printf("Example:./book3 book3 10\n");
    printf("本程序用于测试checkproc的守护进程的小工具。\n");
    printf("procname  进程名称\n");
    printf("timeout  进程的超时时间\n\n");

    return -1;
  }

  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  pActive.AddPInfo(atoi(argv[2]), argv[1]);

  while (1)
  {
    pActive.UptATime();  // 发出心跳

    sleep(15);  // 心跳时间超过超时时间，程序超时
  }

}
