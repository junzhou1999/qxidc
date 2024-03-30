/*
 * 程序名：book07.cpp 本程序用于将线程函数返回给主函数
 * 时间：2024-03-19 14:37
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct st_rets
{
  int icode;         // 线程编号
  char strmsg[31];        // 线程名
};

void *thmain1(void *arg);  // 线程主函数
void *thmain2(void *arg);  // 线程主函数

// 主进程
int main(int argc, char *argv[])
{

  pthread_t thmid1, thmid2;
  thmid1 = thmid2 = 0;

  // 线程id 线程属性 线程主函数 主函数参数
  if (pthread_create(&thmid1, NULL, thmain1, NULL) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid1);
    exit(-1);
  }

  if (pthread_create(&thmid2, NULL, thmain2, NULL) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid1);
    exit(-1);
  }

  void *retcode = NULL;  // 单纯把它看作8字节的变量即可
  struct st_rets *strets = NULL;  // 这是指向地址的指针

  printf("pthread_join ...\n");
  pthread_join(thmid1, &retcode);              // 主进程等待线程
  pthread_join(thmid2, (void **) &strets);     // 主进程等待线程
  printf("pthread_join ok.\n");

  // 二级指针的大小也是8字节
  printf("sizeof(void**)=%ld\n", sizeof(void **));
  if (retcode != NULL)
    printf("retcode=%ld\n", (long) retcode);   // int-> void* -> long

  if (strets != NULL)
  {
    printf("status=%d, msg=%s\n", strets->icode, strets->strmsg);  // struct* -> void* -> struct*
    delete strets;
  }
  return 0;
}

/**
 * 线程主函数
 * @param arg 水桶 水 板砖
 * @return
 */
void *thmain1(void *arg)
{
  printf("这是线程一。\n");
//  return (void*)1151;
  pthread_exit((void *) 1152);
}

void *thmain2(void *arg)
{
  printf("这是线程二。\n");
  // 结构体指针的生命周期需要长过线程函数的运行周期
  struct st_rets *strets = new struct st_rets;
  strets->icode = 404;
  strcpy(strets->strmsg, "Not Found!");
  return (void *) strets;
}