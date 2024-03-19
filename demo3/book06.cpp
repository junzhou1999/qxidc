/*
 * 程序名：book06.cpp 本程序用于将多个参数作为线程函数参数传递过去
 * 时间：2024-03-19 14:37
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct st_args
{
  int no;         // 线程编号
  char name[31];  // 线程名
};

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
  struct st_args *stargs = new struct st_args;
  stargs->no = 10;
  strcpy(stargs->name, "测试线程");
  if (pthread_create(&thmid1, NULL, thmain1, stargs) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid1);
    exit(-1);
  }

  printf("pthread_join ...\n");
  pthread_join(thmid1, NULL);  // 主进程等待线程
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
  struct st_args *stargs = (struct st_args *) arg;
  printf("线程编号：%d，线程名：%s.\n", stargs->no, stargs->name);
  printf("线程编号：%d，线程名：%s.\n", (*stargs).no, (*stargs).name);
  delete stargs;
}