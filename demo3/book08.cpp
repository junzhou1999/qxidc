/*
 * 程序名：book08.cpp 本程序用于将线程函数分离主线程然后回收资源
 * 时间：2024-03-19 14:37
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void *thmain1(void *arg);  // 子线程主函数

// 主线程
int main(int argc, char *argv[])
{

  pthread_t thmid1 = 0;

//  pthread_attr_t pattr;
//  pthread_attr_init(&pattr);
//  pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_DETACHED);

  // 线程id 线程属性 线程主函数 主函数参数
  if (pthread_create(&thmid1, NULL, thmain1, NULL) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid1);
    exit(-1);
  }
//  pthread_detach(thmid1); //线程分离方式1
  sleep(2);  // 即使子进程先退出，但没有分离的情况下，子进程资源还是等待主线程回收


  printf("pthread_join ...\n");

  void *retcode = NULL;
  pthread_join(thmid1, &retcode);           // 分离了的线程由系统管理，主线程join连接不到了，就不会阻塞再这里
  printf("retcode=%ld\n", (long) retcode);
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
  // 用这样分离线程方便
  pthread_detach(pthread_self());

  return (void *) 1151;  // int -> void*
  for (int ii = 0; ii < 5; ii++)
  {
    printf("这是线程一：%d\n", ii);
    sleep(1);
  }
}
