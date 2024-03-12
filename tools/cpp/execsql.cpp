/*
 * 程序名：execsql.cpp 本程序用于执行指定的sql脚本
 * 时间：2024-03-07 14:38
*/
#include "_public.h"
#include "_mysql.h"

CLogFile logfile;  // 日志类对象

CFile file;  // 文件对象，用于读取sql文件

CPActive PActive;  // 进程心跳，超时就会被杀掉

// 信号2和15的处理函数
void EXIT(int sig);

// 数据库的连接
connection conn;

// 站点数据文件 数据库连接（ip,数据库用户，密码，数据库，端口） 字符集 日志
int main(int argc, char *argv[])
{
  // 程序帮助文档
  if (argc != 5)
  {
    printf("\n");
    printf("Using:./execsql sqlfile connstr charset logfile\n");
    printf("Example:/project/tools/bin/execsql /project/idc/sql/cleardata.sql "
           "\"192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306\" utf8 /logs/tools/execsql.log\n");
    printf("        /project/tools/bin/procctl 120 /project/tools/bin/execsql /project/idc/sql/cleardata.sql "
           "\"192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306\" utf8 /logs/tools/execsql.log\n");

    printf("本程序用于执行一次指定的sql文件。由守护程序调度，用法见上边。\n");
    printf("说明：\n");
    printf("sqlfile：用于执行的sql文件位置。\n");
    printf("connstr：数据库的连接配置，包括ip，数据库用户名，密码,连接的数据库，端口号。\n");
    printf("charset：数据库连接字符集。\n");
    printf("logfile：日志的路径文件。\n");
    return -1;
  }

  // 处理信号以及关闭执行到这里的012描述符，需要放在打开日志前
  CloseIOAndSignal(true);
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 打开日志
  if (logfile.Open(argv[4], "a+") == false)
  {
    printf("logfile.Open(%s) falied.\n", argv[4]);
    return -1;
  }

  PActive.AddPInfo(500, "execsql");   // 进程的心跳，一次sql语句查500秒足够
  // 注意，在调试程序的时候，可以启用类似以下的代码，防止超时。
//  PActive.AddPInfo(5000, "execsql");

  // 先打开文件
  if (file.Open(argv[1], "r") == false)
  {
    logfile.Write("file.Open(%s) failed.\n", argv[1]);
    return -1;
  }

  // 连接数据库，自动提交
  if (conn.connecttodb(argv[2], argv[3], 1) != 0)
  {
    logfile.Write("connect database (%s, %s) failed。\n%d:%s\n", argv[2], argv[3], conn.m_cda.rc, conn.m_cda.message);
    return -1;
  }
  logfile.Write("connect database ok.\n");

  sqlstatement stmt(&conn);

  char buffer[1024];
  while (true)
  {
    // 读取每一行
    memset(buffer, 0, 1024);
    if (file.Fgets(buffer, 1023, true) == false) break;  // 要删除换行


    // 删除最后的;
    int ilen = strlen(buffer);
    if (buffer[ilen - 1] != ';') continue;
    buffer[ilen - 1] = '\0';

    // 执行sql语句
    int iret = stmt.execute(buffer);
    if (iret != 0)
    {
      logfile.Write("stmt.execute() failed.\nsql：%s\n%s\n",
                    stmt.m_sql, stmt.m_cda.message);
      continue;
    }
    logfile.Write("exec ok, rpc(%d).\n", stmt.m_cda.rpc);

    PActive.UptATime();  // 执行一次sql语句更新心跳时间
  }

  return 0;
}

void EXIT(int sig)
{
  logfile.Write("收到信号%d，程序退出。\n", sig);
  conn.disconnect();
  exit(0);  // exit函数会析构全局的对象
}