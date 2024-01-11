/*
 * 程序名：deletefiles.cpp 本程序用于删除超过时间的数据文件
 * 时间：2024-01-11 19:58
*/
#include "_public.h"

void EXIT(int sig);

int main(int argc, char *argv[])
{
  // deletefiles dir matchStr timeout
  if (argc != 4)
  {
    printf("Using:./deletefiles pathname matchStr timeout\n");
    printf("Example:/project/tools/bin/deletefiles /tmp/idc *.csv 0.04\n");
    printf(
        "        /project/tools/bin/procctl 300 /project/tools/bin/deletefiles /tmp/idc *.csv,*.xml,*.json 0.04\n\n");

    printf("本程序用于删除超过当前系统timeout天之前在pathname及其子目录的匹配文件。\n");
    printf("pathname 匹配的目录。\n");
    printf("matchStr 简单的\"*\"字符的正则匹配");
    printf("timeout 匹配超时的时间，单位：天数。0.04天代表大约1小时\n\n");
    printf("本程序不写日志文件，也不会在控制台输出任何信息。\n\n");

    return -1;
  }

  // 忽略非9，2，15的信号
  CloseIOAndSignal(true);
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 打开目录
  CDir dir;
  if (dir.OpenDir(argv[1], argv[2], 10000, true) == false)
  {
    printf("dir.OpenDir(%s) failed.\n", argv[1]);
    return -1;
  }

  // 获取当前时间
  char strTimeOut[21];
  LocalTime(strTimeOut, "yyyy-mm-dd hh24:mi:ss", 0 - (int) (atof(argv[3]) * 24 * 60 * 60));

  // 循环读取目录
  while (1)
  {
    // 读完就跳出循环
    if (dir.ReadDir() == false) break;

    // 超时，调用系统命令压缩文件
    if (strcmp(dir.m_ModifyTime, strTimeOut) < 0)
    {
      if ((REMOVE(dir.m_FullFileName)) == false)
        printf("REMOVE(%s) failed.\n", dir.m_FullFileName);
      else
        printf("REMOVE(%s) ok.\n", dir.m_FullFileName);
    }
  }
  return 0;
}

void EXIT(int sig)
{
  printf("收到信号%d，程序退出。\n", sig);
  exit(0);
}

