/*
 * 程序名：obtmindtodb.cpp 本程序用于将全国气象站点分钟观测的数据加载到数据库中，暂支持csv
 * 时间：2024-03-07 14:38
*/
#include "_public.h"
#include "_mysql.h"

CLogFile logfile;  // 日志类对象

CDir dir;    // 读取采集数据的目录
CFile file;  // 文件对象，用于读取站点观测数据文件

CPActive PActive;  // 进程心跳，超时就会被杀掉

struct st_zhobtmind
{
  char obtid[11];       // 站点代码。
  char ddatetime[21];   // 数据时间，精确到分钟。
  char t[11];           // 温度，单位：0.1摄氏度。
  char p[11];           // 气压：单位：0.1千帕。
  char u[11];           // 相对湿度，0-100之间的值。
  char wd[11];          // 风向，0-360之间的值。
  char wf[11];          // 风速，单位：0.1m/s。
  char r[11];           // 降雨量，单位：0.1mm。
  char vis[11];         // 能见度，单位：0.1米。
};

// 信号2和15的处理函数
void EXIT(int sig);

// 数据库的连接
connection conn;

// 主函数
bool _obtmindtodb(const char *datapath, char *connstr, char *charset);

// 观测数据的目录 数据库连接（ip,数据库用户，密码，数据库，端口） 字符集 日志
int main(int argc, char *argv[])
{
  // 程序帮助文档
  if (argc != 5)
  {
    printf("\n");
    printf("Using:./obtmindtodb datapath connstr connEncoding logfile\n");
    printf("Example:/project/idc/bin/obtcodetodb /tmp/idc/surfdata "
           "\"192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306\" utf8 /logs/idc/obtmindtodb.log\n");
    printf("        /project/tools/bin/procctl 120 /project/idc/bin/obtmindtodb /tmp/idc/surfdata "
           "\"192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306\" utf8 /logs/idc/obtmindtodb.log\n");

    printf("本程序用于将全国气象站点的分钟观测的加载到数据库中。由守护程序调度，用法见上边。\n");
    printf("说明：\n");
    printf("datapath：全国气象站点的分钟观测数据的路径。\n");
    printf("connstr：数据库的连接配置；包括 ip,user,userpwd,db,port\n");
    printf("connEncoding：数据库连接字符集。\n");
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

//  PActive.AddPInfo(10, "obtmindtodb");   // 进程的心跳，10秒足够。
  // 注意，在调试程序的时候，可以启用类似以下的代码，防止超时。
  PActive.AddPInfo(5000, "obtmindtodb");

  // 主函数
  _obtmindtodb(argv[1], argv[2], argv[3]);

  return 0;
}

/**
 * 分钟观测数据入库的主函数
 * @return
 */
bool _obtmindtodb(const char *datapath, char *connstr, char *charset)
{
  // 打开数据目录
  if (dir.OpenDir(datapath, "*.xml") == false)
  {
    logfile.Write("dir.OpenDir(%s) failed.\n", datapath);
    return false;
  }

  CTimer timer;
  int totalcount = 0;   // 文件的总记录数
  int inscount = 0;     // 插入的元组数

  char buffer[1001];   // 行内容的buffer
  sqlstatement stmt;   // 执行sql语句的对象
  struct st_zhobtmind stzhobtmind;  // 写入数据库的结构体

  // 循环遍历每个文件，获得文件名
  // 读文件的循环
  while (true)
  {
    // 读到了文件列表的末尾
    if (dir.ReadDir() == false) break;

    // timer.Start();  Elapsed()会重新计时
    totalcount = inscount = 0;

    // 打开文件
    if (file.Open(dir.m_FullFileName, "r") == false)
    {
      logfile.Write("file.Open(%s) failed.\n", dir.m_FullFileName);
      return false;
    }

    // 连接数据库的时机应该是读取到了文件内容
    if (conn.m_state == 0)
    {
      // 连接数据库
      if (conn.connecttodb(connstr, charset) != 0)
      {
        logfile.Write("连接数据库(%s, %s)出错。\n%s\n", connstr, charset, conn.m_cda.message);
        return -1;
      }
    }

    stmt.connect(&conn);  // stmt需要配置连接

    stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis,upttime) "
                 "values(:1,str_to_date(:2,'%%Y%%m%%d%%H%%i%%s'),:3,:4,:5,:6,:7,:8,:9,now())");
    stmt.bindin(1, stzhobtmind.obtid, 10);
    stmt.bindin(2, stzhobtmind.ddatetime, 14);
    stmt.bindin(3, stzhobtmind.t, 10);
    stmt.bindin(4, stzhobtmind.p, 10);
    stmt.bindin(5, stzhobtmind.u, 10);
    stmt.bindin(6, stzhobtmind.wd, 10);
    stmt.bindin(7, stzhobtmind.wf, 10);
    stmt.bindin(8, stzhobtmind.r, 10);
    stmt.bindin(9, stzhobtmind.vis, 10);

    // 读行内容的循环
    while (true)
    {
      // 将每一行的内容解析到结构体中
      if (file.FFGETS(buffer, 1000, "<endl/>") == false) break;
      totalcount++;

      GetXMLBuffer(buffer, "obtid", stzhobtmind.obtid, 10);
      GetXMLBuffer(buffer, "ddatetime", stzhobtmind.ddatetime, 14);
      char strTmp[11];  // 对文件数据做数据库入库的数据处理
      GetXMLBuffer(buffer, "t", strTmp);
      if (strlen(strTmp) > 0) snprintf(stzhobtmind.t, 10, "%d", int(atof(strTmp) * 10));
      GetXMLBuffer(buffer, "p", strTmp);
      if (strlen(strTmp) > 0) snprintf(stzhobtmind.p, 10, "%d", int(atof(strTmp) * 10));
      GetXMLBuffer(buffer, "u", strTmp);
      if (strlen(strTmp) > 0) snprintf(stzhobtmind.u, 10, "%d", int(atof(strTmp) * 10));
      GetXMLBuffer(buffer, "wd", strTmp);
      if (strlen(strTmp) > 0) snprintf(stzhobtmind.wd, 10, "%d", int(atof(strTmp) * 10));
      GetXMLBuffer(buffer, "wf", strTmp);
      if (strlen(strTmp) > 0) snprintf(stzhobtmind.wf, 10, "%d", int(atof(strTmp) * 10));
      GetXMLBuffer(buffer, "r", strTmp);
      if (strlen(strTmp) > 0) snprintf(stzhobtmind.r, 10, "%d", int(atof(strTmp) * 10));
      GetXMLBuffer(buffer, "vis", strTmp);
      if (strlen(strTmp) > 0) snprintf(stzhobtmind.vis, 10, "%d", int(atof(strTmp) * 10));


      if (stmt.execute() != 0)
      {
        // 1、失败的情况有哪些？是否全部的失败都要写日志？
        // 答：失败的原因主要有二：一是记录重复，二是数据内容非法。
        // 2、如果失败了怎么办？程序是否需要继续？是否rollback？是否返回false？
        // 答：如果失败的原因是数据内容非法，记录日志后继续；如果是记录重复，不必记录日志，且继续。
        if (stmt.m_cda.rc != 1062)
        {
          logfile.Write("stmt.execute() failed.\n%s\n%d：%s\n", stmt.m_sql, stmt.m_cda.rc, stmt.m_cda.message);
          logfile.Write("content：%s\n", buffer);
        }
      }
      else inscount++;
    }

    // 提交事务
    conn.commit();
    logfile.Write("文件：%s，处理的记录数为：%d，插入的元组为：%d，耗时：%.2lf秒\n", dir.m_FullFileName, totalcount, inscount,
                  timer.Elapsed());
  }

  return true;
}

void EXIT(int sig)
{
  logfile.Write("收到信号%d，程序退出。\n", sig);
  conn.disconnect();
  exit(0);  // exit函数会析构全局的对象
}