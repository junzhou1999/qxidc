/*
 * 程序名：obtmindtodb.cpp 本程序用于将全国气象站点分钟观测的数据加载到数据库中，支持csv、xml的解析
 * 时间：2024-03-07 14:38
*/
#include "_public.h"
#include "_mysql.h"

CLogFile logfile;  // 日志类对象

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

// 整合的一个类
class CZHOBTMIND
{
  // 成员
private:
  // 需要初始化的
  CLogFile *m_logFile;   // 日志
  connection *m_conn;    // 数据库连接

  char m_buffer[1024];   // 读取到的行，用于显示错误的行内容
  sqlstatement m_stmt;   // sql执行
  struct st_zhobtmind m_zhobtmind;

public:
  void Bindconnlog(connection *conn, CLogFile *logfile);   // 初始化绑定
  bool SplitBuffer(char *strBuffer, bool isxml = true);      // 把获取到的文件内容写入到结构体中
  bool InsertTable();                                      // stmt插入数据库中


  CZHOBTMIND();

  CZHOBTMIND(connection *conn, CLogFile *logfile);

  ~CZHOBTMIND();
};

CZHOBTMIND::CZHOBTMIND()
{
  m_logFile = 0;
  m_conn = 0;
}

CZHOBTMIND::CZHOBTMIND(connection *conn, CLogFile *logfile)
{
  Bindconnlog(conn, logfile);
}

CZHOBTMIND::~CZHOBTMIND()
{}

/**
 * 初始化成员
 * @param conn
 * @param logfile
 */
void CZHOBTMIND::Bindconnlog(connection *conn, CLogFile *logfile)
{
  m_logFile = logfile;
  m_conn = conn;
}

/**
 * 把获取的每一行XML内容给弄到结构体中
 * @param strBuffer
 * bisxml：buffer格式是否为xml，true：xml，false：json；默认：true
 * @return
 */
bool CZHOBTMIND::SplitBuffer(char *strBuffer, bool bisxml)
{
  // 把获取到的每一行内容弄到成员变量来
  STRNCPY(m_buffer, 1024, strBuffer, 1023);
  memset(&m_zhobtmind, 0, sizeof(struct st_zhobtmind));

  if (bisxml == false)
  {
    if (strstr(m_buffer, "站点")) return false;
    CCmdStr cmdStr(m_buffer, ",");
    cmdStr.GetValue(0, m_zhobtmind.obtid, 10);
    cmdStr.GetValue(1, m_zhobtmind.ddatetime, 14);
    char strTmp[11];  // 对文件数据做数据库入库的数据处理
    cmdStr.GetValue(2, strTmp, 10);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.t, 10, "%d", int(atof(strTmp) * 10));
    cmdStr.GetValue(3, strTmp, 10);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.p, 10, "%d", int(atof(strTmp) * 10));
    cmdStr.GetValue(4, strTmp, 10);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.u, 10, "%d", int(atof(strTmp) * 10));
    cmdStr.GetValue(5, strTmp, 10);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.wd, 10, "%d", int(atof(strTmp) * 10));
    cmdStr.GetValue(6, strTmp, 10);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.wf, 10, "%d", int(atof(strTmp) * 10));
    cmdStr.GetValue(7, strTmp, 10);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.r, 10, "%d", int(atof(strTmp) * 10));
    cmdStr.GetValue(8, strTmp, 10);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.vis, 10, "%d", int(atof(strTmp) * 10));
  }
  else
  {
    GetXMLBuffer(m_buffer, "obtid", m_zhobtmind.obtid, 10);
    GetXMLBuffer(m_buffer, "ddatetime", m_zhobtmind.ddatetime, 14);
    char strTmp[11];  // 对文件数据做数据库入库的数据处理
    GetXMLBuffer(m_buffer, "t", strTmp);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.t, 10, "%d", int(atof(strTmp) * 10));
    GetXMLBuffer(m_buffer, "p", strTmp);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.p, 10, "%d", int(atof(strTmp) * 10));
    GetXMLBuffer(m_buffer, "u", strTmp);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.u, 10, "%d", int(atof(strTmp) * 10));
    GetXMLBuffer(m_buffer, "wd", strTmp);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.wd, 10, "%d", int(atof(strTmp) * 10));
    GetXMLBuffer(m_buffer, "wf", strTmp);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.wf, 10, "%d", int(atof(strTmp) * 10));
    GetXMLBuffer(m_buffer, "r", strTmp);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.r, 10, "%d", int(atof(strTmp) * 10));
    GetXMLBuffer(m_buffer, "vis", strTmp);
    if (strlen(strTmp) > 0) snprintf(m_zhobtmind.vis, 10, "%d", int(atof(strTmp) * 10));
  }
  return true;
}

/**
 * 配置statement对象连接，然后将sql语句执行了
 * @return false代表没有插入
 */
bool CZHOBTMIND::InsertTable()
{
  // 重新配置stmt
  if (m_stmt.m_state == 0)
  {
    m_stmt.connect(m_conn);  // stmt需要配置连接

    m_stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis,upttime) "
                   "values(:1,str_to_date(:2,'%%Y%%m%%d%%H%%i%%s'),:3,:4,:5,:6,:7,:8,:9,now())");
    m_stmt.bindin(1, m_zhobtmind.obtid, 10);
    m_stmt.bindin(2, m_zhobtmind.ddatetime, 14);
    m_stmt.bindin(3, m_zhobtmind.t, 10);
    m_stmt.bindin(4, m_zhobtmind.p, 10);
    m_stmt.bindin(5, m_zhobtmind.u, 10);
    m_stmt.bindin(6, m_zhobtmind.wd, 10);
    m_stmt.bindin(7, m_zhobtmind.wf, 10);
    m_stmt.bindin(8, m_zhobtmind.r, 10);
    m_stmt.bindin(9, m_zhobtmind.vis, 10);
  }

  if (m_stmt.execute() != 0)
  {
    // 1、失败的情况有哪些？是否全部的失败都要写日志？
    // 答：失败的原因主要有二：一是记录重复，二是数据内容非法。
    // 2、如果失败了怎么办？程序是否需要继续？是否rollback？是否返回false？
    // 答：如果失败的原因是数据内容非法，记录日志后继续；如果是记录重复，不必记录日志，且继续。
    if (m_stmt.m_cda.rc != 1062)
    {
      m_logFile->Write("m_stmt.execute() failed.\n%s\n%d：%s\n", m_stmt.m_sql, m_stmt.m_cda.rc, m_stmt.m_cda.message);
      m_logFile->Write("行数据：%s 有误。\n", m_buffer);
    }
    return false;
  }
  return true;
}

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
//  PActive.AddPInfo(5000, "obtmindtodb");

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
  CDir dir;    // 读取采集数据的目录
  CFile file;  // 文件对象，用于读取站点观测数据文件

  // 打开数据目录
  if (dir.OpenDir(datapath, "*.xml,*.csv") == false)
  {
    logfile.Write("dir.OpenDir(%s) failed.\n", datapath);
    return false;
  }

  // 配置接下来的解析和insert
  CZHOBTMIND zhobtmind(&conn, &logfile);

  char strBuffer[1024]; // 读取行的buffer
  bool bisxml = true;
  CTimer timer;         // 计时器
  int totalcount = 0;   // 文件的总记录数
  int inscount = 0;     // 插入的元组数


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
      continue;
    }

    // 连接数据库的时机应该是读取到了文件内容
    if (conn.m_state == 0)
    {
      // 连接数据库
      if (conn.connecttodb(connstr, charset) != 0)
      {
        logfile.Write("连接数据库(%s, %s)出错。\n%s\n", connstr, charset, conn.m_cda.message);
        return false;
      }
      logfile.Write("连接数据库ok。\n");
    }

    // 读行内容的循环
    while (true)
    {
      // 将每一行的内容解析到结构体中
      if (MatchStr(dir.m_FileName, "*.xml"))
      {
        // 会执行清空buffer
        if (file.FFGETS(strBuffer, 1023, "<endl/>") == false) break;
        bisxml = true;
      }
      else
      {
        if (file.Fgets(strBuffer, 1023, true) == false) break;
        bisxml = false;
      }

      // 1
      if (zhobtmind.SplitBuffer(strBuffer, bisxml) == false) continue;

      totalcount++;

      // 2
      if (zhobtmind.InsertTable() == true) inscount++;
    }

    // 提交事务
    conn.commit();
    logfile.Write("文件：%s，处理的记录数为：%d，插入的元组为：%d，耗时：%.2lf秒\n", dir.m_FullFileName,
                  totalcount, inscount, timer.Elapsed());
  }

  return true;
}

void EXIT(int sig)
{
  logfile.Write("收到信号%d，程序退出。\n", sig);
  conn.disconnect();
  exit(0);  // exit函数会析构全局的对象
}