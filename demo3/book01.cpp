/*
 * 程序名：book01.cpp 本程序用于
 * 时间：2024-03-19 14:37
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void *thmain(void *arg);  // 线程主函数

// 主进程
int main(int argc, char *argv[])
{

  pthread_t thmid;  // typedef unsigned long pthread_t
  // 线程id 线程属性 线程主函数 主函数参数
  if (pthread_create(&thmid, NULL, thmain, NULL) != 0)
  {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  printf("pthread_join ...\n");
//  sleep(10);  // 如果主进程先退出，那么线程也会退出（没有join或者其他阻塞的情况下）
  pthread_join(thmid, NULL);  // 主进程等待线程，相当于父进程wait子进程
  printf("pthread_join ok.\n");

  return 0;
}

// 线程主函数
void *thmain(void *arg)
{
  for (int ii = 0; ii < 5; ii++)
  {
    printf("线程输出：%d\n", ii);
    sleep(1);
  }
}