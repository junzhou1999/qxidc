/*
 * 程序名：book01.cpp 本程序用于
 * 时间：2024-03-19 14:37
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int globalvar = 0;

void *thmain1(void *arg);  // 线程主函数
void *thmain2(void *arg);  // 线程主函数

// 主进程
int main(int argc, char *argv[])
{

  pthread_t thmid1, thmid2;
  thmid1 = thmid2 = -1;
  // 线程id 线程属性 线程主函数 主函数参数
  if (pthread_create(&thmid1, NULL, thmain1, NULL) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid1);
    exit(-1);
  }

  // 创建两个线程
  if (pthread_create(&thmid2, NULL, thmain2, NULL) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid2);
    exit(-1);
  }

  printf("pthread_join ...\n");

  // 可以在其他线程内调用线程退出
  // sleep(2);  pthread_cancel(thmid2);

  pthread_join(thmid1, NULL);  // 主进程等待线程
  pthread_join(thmid2, NULL);  // 主进程等待线程
  printf("pthread_join ok.\n");

  return 0;
}

void func1()
{ return; }  // 这个只会返回函数
void func2()
{ pthread_exit(0); }  // 这个会退出线程

// 线程主函数
void *thmain1(void *arg)
{
  for (int ii = 0; ii < 5; ii++)
  {
    printf("线程一输出：%d\n", ii);
    sleep(1);
//    if (ii==2)  pthread_exit((void*)1);
//    if (ii==2)  return (void*)1;
//    if (ii==2)  func1();
    if (ii == 2) func2();
  }
}

// 线程主函数
void *thmain2(void *arg)
{
  for (int ii = 0; ii < 5; ii++)
  {
    printf("线程二输出：%d\n", ii);
    sleep(1);
  }
}