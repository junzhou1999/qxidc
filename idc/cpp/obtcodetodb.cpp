/*
 * 程序名：obtcodetodb.cpp 本程序用于将全国气象站点的数据加载到数据库中
 * 时间：2024-03-07 14:38
*/
#include "_public.h"
#include "_mysql.h"

CLogFile logfile;  // 日志类对象

CFile file;  // 文件对象，用于读取站点数据文件

CPActive PActive;  // 进程心跳，超时就会被杀掉

// 信号2和15的处理函数
void EXIT(int sig);

// 站点结构体
struct st_stcode
{
  char provname[31];   // 省
  char obtid[11];      // 站号
  char obtname[31];    // 站名
  char lat[11];        // 纬度
  char lon[11];        // 经度
  char height[11];     // 海拔高度
};

// 存储站点内容的容器
vector<struct st_stcode> vstcode;

// 加载站点内容文件
bool LoadStCode(const char *inifile);

// 数据库的连接
connection conn;

// 站点数据文件 数据库连接（ip,数据库用户，密码，数据库，端口） 字符集 日志
int main(int argc, char *argv[])
{
  // 程序帮助文档
  if (argc != 5)
  {
    printf("\n");
    printf("Using:./obtcodetodb inifile ip,user,userpwd,db,port connectionEncoding logfile\n");
    printf("Example:/project/idc/bin/obtcodetodb /project/idc/ini/stcode.ini "
           "\"192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306\" utf8 /logs/idc/obtcodetodb.log\n");
    printf("        /project/tools/bin/procctl 120 /project/idc/bin/obtcodetodb /project/idc/ini/stcode.ini "
           "\"192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306\" utf8 /logs/idc/obtcodetodb.log\n");

    printf("本程序用于本程序用于将全国气象站点的数据文件加载到数据库中。由守护程序调度，用法见上边。\n");
    printf("说明：\n");
    printf("inifile：全国气象站点的数据文件的路径文件。\n");
    printf("ip,user,userpwd,db,port：数据库的连接配置。\n");
    printf("connectionEncoding：数据库连接字符集。\n");
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

  PActive.AddPInfo(10, "obtcodetodb");   // 进程的心跳，10秒足够。
  // 注意，在调试程序的时候，可以启用类似以下的代码，防止超时。
  // PActive.AddPInfo(5000,"obtcodetodb");

  // 将文件写入到容器中
  if (LoadStCode(argv[1]) == false)
  {
    logfile.Write("LoadStCode(%s) failed.\n", argv[1]);
    return -1;
  }

  // 连接数据库
  if (conn.connecttodb(argv[2], argv[3]) != 0)
  {
    logfile.Write("连接数据库(%s, %s)出错。\n%s\n", argv[2], argv[3], conn.m_cda.message);
    return -1;
  }

  struct st_stcode stcode;

  // 创建ins和upt prepare语句
  sqlstatement stmtins(&conn);
  stmtins.prepare("insert into T_ZHOBTCODE(obtid,cityname,provname,lat,lon,height,upttime) "
                  "values(:1,:2,:3,:4*100,:5*100,:6*10,now())");
  stmtins.bindin(1, stcode.obtid, 10);
  stmtins.bindin(2, stcode.obtname, 30);
  stmtins.bindin(3, stcode.provname, 30);
  stmtins.bindin(4, stcode.lat, 10);
  stmtins.bindin(5, stcode.lon, 10);
  stmtins.bindin(6, stcode.height, 10);

  sqlstatement stmtupt(&conn);
  stmtupt.prepare("update T_ZHOBTCODE set cityname=:1,provname=:2,lat=:3*100,lon=:4*1000,height=:5*10,upttime=now()"
                  "where obtid=:6");
  stmtupt.bindin(1, stcode.obtname, 30);
  stmtupt.bindin(2, stcode.provname, 30);
  stmtupt.bindin(3, stcode.lat, 10);
  stmtupt.bindin(4, stcode.lon, 10);
  stmtupt.bindin(5, stcode.height, 10);
  stmtupt.bindin(6, stcode.obtid, 10);

  CTimer timer;
  int iinscount = 0, iuptcount = 0;

  for (int ii = 0; ii < vstcode.size(); ii++)
  {
    // 拷贝到一个结构体里
    memcpy(&stcode, &vstcode[ii], sizeof(struct st_stcode));

    // 插入不成功；1062-已有同主键的记录；其他非0-错误
    if (stmtins.execute() != 0)
    {
      // 已有数据，更新操作
      if (stmtins.m_cda.rc == 1062)
      {
        // 更新也错了
        if (stmtupt.execute() != 0)
        {
          logfile.Write("stmtupt.execute() failed.\n%s\n%s\n", stmtupt.m_sql, stmtupt.m_cda.message);
          return -1;
        }
        iuptcount++;
      }
      else
      {
        // 插入错误
        logfile.Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
        return -1;
      }
    }
    else iinscount++;
  }

  logfile.Write("本次操作的记录数为：%d，新插入的元组数：%d, 更新的元组数：%d。耗时：%.2lf秒\n",
                vstcode.size(), iinscount, iuptcount, timer.Elapsed());

  // 提交事务
  conn.commit();

  return 0;
}

/**
 * 加载站点文件
 * @param inifile  输入站点的文件
 * @return
 */
bool LoadStCode(const char *inifile)
{
  // 打开文件
  if (file.Open(inifile, "r") == false)
  {
    logfile.Write("站点参数文件打开失败。\n");
    return false;
  }

  char buffer[301];           // 读取一行数据的缓存
  struct st_stcode stcode;    // 读取的一整个站点数据
  CCmdStr cmdStr;             // 分割字符串

  while (true)
  {
    // 清空缓存
    memset(buffer, 0, sizeof(buffer));
    memset(&stcode, 0, sizeof(struct st_stcode));

    // 读取一行
    if (file.Fgets(buffer, 300) == false) break;

    // 分割字符串
    cmdStr.SplitToCmd(buffer, ",", true);
    if (cmdStr.CmdCount() != 6) continue;
    cmdStr.GetValue(0, stcode.provname, 30);  // ilen：读取出来的数据不能超出程序所能存储的大小
    cmdStr.GetValue(1, stcode.obtid, 10);
    cmdStr.GetValue(2, stcode.obtname, 30);
    cmdStr.GetValue(3, stcode.lat, 10);
    cmdStr.GetValue(4, stcode.lon, 10);
    cmdStr.GetValue(5, stcode.height, 10);

    // 写入程序容器
    vstcode.push_back(stcode);
  }

  return true;
}

void EXIT(int sig)
{
  logfile.Write("收到信号%d，程序退出。\n", sig);
  conn.disconnect();
  exit(0);  // exit函数会析构全局的对象
}