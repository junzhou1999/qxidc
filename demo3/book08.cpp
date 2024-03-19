/*
 * 程序名：book08.cpp 本程序用于将线程函数分离主线程然后回收资源
 * 时间：2024-03-19 14:37
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

class Connection
{
public:
  Connection()
  { printf("数据库连接对象的初始化。\n"); };

  ~Connection()
  { printf("数据库连接对象的析构。\n"); };
};

void *thmain1(void *arg);            // 子线程主函数
void thcleanup1(void *arg);         // 线程清理函数，固定格式
void thcleanup2(void *arg);          // 线程清理函数，固定格式

// 主线程
int main(int argc, char *argv[])
{

  pthread_t thmid1 = 0;

  // 线程id 线程属性 线程主函数 主函数参数
  if (pthread_create(&thmid1, NULL, thmain1, NULL) != 0)
  {
    printf("pthread_create(%lu) failed.\n", thmid1);
    exit(-1);
  }
  // 主线程干点别的
  sleep(10);

  return 0;
}

/**
 * 线程主函数
 * @param arg 水桶 水 板砖
 * @return
 */
void *thmain1(void *arg)
{
  // 分离线程
  pthread_detach(pthread_self());

  for (int ii = 0; ii < 2; ii++)
  {
    printf("这是线程一：%d\n", ii);
    sleep(1);
  }

  Connection *conn = new Connection();

  // 线程分离后需要自主回收资源
  pthread_cleanup_push(thcleanup1, conn);
    pthread_cleanup_push(thcleanup2, NULL);
    pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  return (void *) 1151;  // int -> void*
}

void thcleanup1(void *arg)
{
  if (arg != NULL)
    delete (Connection *) arg;
}

void thcleanup2(void *arg)
{
  printf("清理文件，网络socket... ");
  printf("ok.\n");
}

