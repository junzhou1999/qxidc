/*
 * 程序名：crtsurfdata.cpp 本程序用于生成全国气象站点观测的分钟数据
 * 时间：2024-01-05 18:03
*/

#include "_public.h"

CLogFile logfile;

int main(int argc, char *argv[])
{
  // ini outpath log
  if (argc!=4)
  {
    printf("Using:./crtsurfdata inifile outpath logfile\n");
    printf("Example:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc /logs/idc/crtsurfdata.log\n");

    printf("inifile 全国气象站点参数文件名。\n");
    printf("outpath 全国气象站点观测数据文件存放的目录。\n");
    printf("logfile 本程序运行的日志文件名\n\n");
    return -1;
  }

  if (logfile.Open(argv[3]) == false)
  {
    printf("logfile.Open(%s) failed.\n", argv[3]);
    return -1;
  }

  logfile.Write("crtsurfdata 开始运行。\n");

  logfile.Write("crtsurfdata 结束运行。\n");

  return 0;
}
