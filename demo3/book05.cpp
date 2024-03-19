/*
 * 程序名：book05.cpp 本程序用于将动态内存的变量地址作为线程函数参数传递过去
 * 时间：2024-03-19 14:37
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void *thmain1(void *arg);  // 线程主函数
void *thmain2(void *arg);  // 线程主函数
void *thmain3(void *arg);  // 线程主函数
void *thmain4(void *arg);  // 线程主函数
void *thmain5(void *arg);  // 线程主函数

// 主进程
int main(int argc, char *argv[])
{

  pthread_t thmid1, thmid2, thmid3, thmid4, thmid5;
  thmid1 = thmid2 = thmid3 = thmid4 = thmid5 = 0;


  // 线程id 线程属性 线程主函数 主函数参数
  int *pi1 = new int(1);
  if (pthread_create(&thmid1, NULL, thmain1, (void *) pi1) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid1);
    exit(-1);
  }

  int *pi2 = new int(2);
  // void*兼容其他指针类型，表示只关心地址本身，不关心其中的内容
  if (pthread_create(&thmid2, NULL, thmain2, pi2) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid2);
    exit(-1);
  }

  int *pi3 = new int(3);
  if (pthread_create(&thmid3, NULL, thmain3, pi3) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid3);
    exit(-1);
  }

  int *pi4 = new int(4);
  if (pthread_create(&thmid4, NULL, thmain4, pi4) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid4);
    exit(-1);
  }

  int *pi5 = new int(5);
  if (pthread_create(&thmid5, NULL, thmain5, pi5) != 0)
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
  printf("线程一输出参数：%d\n", *(int *) arg);  // 传来地址了，就要解引用
  delete (int *) arg;  // 动态内存在线程中释放
}

void *thmain2(void *arg)
{
  printf("线程二输出参数：%d\n", *(int *) arg);
  delete (int *) arg;
}

void *thmain3(void *arg)
{
  printf("线程三输出参数：%d\n", *(int *) arg);
  delete (int *) arg;
}

void *thmain4(void *arg)
{
  printf("线程四输出参数：%d\n", *(int *) arg);
  delete (int *) arg;
}

void *thmain5(void *arg)
{
  printf("线程五输出参数：%d\n", *(int *) arg);
  delete (int *) arg;
}