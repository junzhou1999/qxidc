/*
 * 程序名：book1.cpp 本程序用于测试新建一个共享内存
 * 时间：2024-01-10 13:56
*/
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Using:./book1 content\n");
    printf("content 往共享内存写入的字节。\n\n");
    return -1;
  }
  int shmid = 0;  // 共享内存id
  // 创建1024字节空间大小的共享内存，如果存在则获取它
  if ((shmid = shmget(key_t(0x5005), 1024, 0666 | IPC_CREAT)) == -1)
  {
    printf("shmget(0x5005) failed.\n");
    return -1;
  }

  // 程序连接共享内存
  char *ptext = 0;
  ptext = (char *) shmat(shmid, 0, 0);

  // 读取共享内存
  printf("操作前的共享内存内容：%s\n", ptext);

  // 修改共享内存
  strncpy(ptext, argv[1], 1024);

  // 读取修改后的共享内存
  printf("操作后的共享内存内容：%s\n", ptext);

  // 断连共享内存
  shmdt(ptext);

  return 0;
}
