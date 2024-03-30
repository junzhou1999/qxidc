/*
 * 程序名：book04.cpp 本程序用于将8字节的内存参数作为线程函数参数传递
 * 时间：2024-03-19 14:37
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int globalvar = 0;

void *thmain1(void *arg);  // 线程主函数
void *thmain2(void *arg);  // 线程主函数
void *thmain3(void *arg);  // 线程主函数
void *thmain4(void *arg);  // 线程主函数
void *thmain5(void *arg);  // 线程主函数

// 主进程
int main(int argc, char *argv[])
{

  pthread_t thmid1, thmid2, thmid3, thmid4, thmid5;

  // 线程id 线程属性 线程主函数 主函数参数
  globalvar = 1;
  // 这里存进去的这是long型的8字节void*空间
  if (pthread_create(&thmid1, NULL, thmain1, (void *) (long) globalvar) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid1);
    exit(-1);
  }

  globalvar = 2;
  if (pthread_create(&thmid2, NULL, thmain2, (void *) (long) globalvar) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid2);
    exit(-1);
  }

  globalvar = 3;
  if (pthread_create(&thmid3, NULL, thmain3, (void *) (long) globalvar) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid3);
    exit(-1);
  }

  globalvar = 4;
  if (pthread_create(&thmid4, NULL, thmain4, (void *) (long) globalvar) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid4);
    exit(-1);
  }

  globalvar = 5;
  if (pthread_create(&thmid5, NULL, thmain5, (void *) (long) globalvar) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid5);
    exit(-1);
  }

  printf("pthread_join ...\n");
  pthread_join(thmid1, NULL);  // 主进程等待线程
  pthread_join(thmid2, NULL);  // 主进程等待线程
  pthread_join(thmid3, NULL);  // 主进程等待线程
  pthread_join(thmid4, NULL);  // 主进程等待线程
  pthread_join(thmid5, NULL);  // 主进程等待线程
  printf("pthread_join ok.\n");

  return 0;
}

/**
 * 线程主函数
 * @param arg 水桶 水 板砖
 * @return
 */
void *thmain1(void *arg)
{
  printf("线程一输出参数：%d\n", (int) (long) arg);  // 它只是一个空间，不用解引用
}

void *thmain2(void *arg)
{
  printf("线程二输出参数：%d\n", (int) (long) arg);
}

void *thmain3(void *arg)
{
  printf("线程三输出参数：%d\n", (int) (long) arg);
}

void *thmain4(void *arg)
{
  printf("线程四输出参数：%d\n", (int) (long) arg);
}

void *thmain5(void *arg)
{
  printf("线程五输出参数：%d\n", (int) (long) arg);
}