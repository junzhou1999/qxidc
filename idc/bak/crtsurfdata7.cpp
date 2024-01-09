/*
 * 程序名：crtsurfdata.cpp 本程序用于生成全国气象站点观测的分钟数据
 * 时间：2024-01-05 18:03
*/

#include "_public.h"

CLogFile logfile;

// 站点结构体
struct st_stcode
{
  char provname[31]; // 省
  char obtid[11];    // 站号
  char obtname[31];  // 站名
  double lat;        // 纬度
  double lon;        // 经度
  double height;     // 海拔高度
};

// 存储站点内容的容器
vector<struct st_stcode> vstcode;

// 加载站点内容文件
bool LoadStCode(const char *inifile);

// 每个站点的模拟观测数据
struct st_surfdata
{
  char obtid[11];      // 站点代码。
  char ddatetime[21];  // 数据时间：格式yyyymmddhh24miss
  int t;              // 气温：单位，0.1摄氏度。
  int p;              // 气压：0.1百帕。
  int u;              // 相对湿度，0-100之间的值。
  int wd;             // 风向，0-360之间的值。
  int wf;             // 风速：单位0.1m/s
  int r;              // 降雨量：0.1mm。
  int vis;            // 能见度：0.1米。
};

// 每次所有站点观测数据的容器
vector<struct st_surfdata> vsurfdata;

char strddatetime[21];  // 当次观测时间

// 生成模拟的观测数据
void CrtSurfData();

// 观测数据写入到文件中
bool CrtSurfFile(const char *outpath, const char *datafmt);

int main(int argc, char *argv[])
{
  // ini outpath log datafmt [datetime]
  if (argc != 5 && argc != 6)
  {
    printf("Using:./crtsurfdata inifile outpath logfile datafmt [datetime]\n");
    printf(
        "Example:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc /logs/idc/crtsurfdata.log csv,xml,json\n");
    printf(
        "Example:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc /logs/idc/crtsurfdata.log csv 20240101120000\n");

    printf("inifile 全国气象站点参数文件名。\n");
    printf("outpath 全国气象站点观测数据文件存放的目录。\n");
    printf("logfile 本程序运行的日志文件名\n");
    printf("datafmt 生成数据文件的格式，支持xml、json和csv三种格式，中间用逗号分隔。\n");
    printf("datetime 这是一个可选参数，表示生成指定时间的数据和文件，必须是\"yyyymmddhh24miss\"14位时间格式\n\n");

    return -1;
  }

  if (logfile.Open(argv[3]) == false)
  {
    printf("logfile.Open(%s) failed.\n", argv[3]);
    return -1;
  }

  logfile.Write("crtsurfdata 开始运行。\n");

  // 加载站点文件到程序容器中
  if (LoadStCode(argv[1]) != true)
  {
    logfile.Write("加载站点文件失败。\n");
    return -1;
  }

  if (argc == 5) LocalTime(strddatetime, "yyyymmddhh24miss");
  if (argc == 6) STRCPY(strddatetime, sizeof(strddatetime), argv[5]);

  // 生成模拟观测数据，填充容器
  CrtSurfData();

  // 将观测数据写入到文件
  if (strstr(argv[4], "csv")) CrtSurfFile(argv[2], "csv");
  if (strstr(argv[4], "xml")) CrtSurfFile(argv[2], "xml");
  if (strstr(argv[4], "json")) CrtSurfFile(argv[2], "json");

  logfile.Write("crtsurfdata 结束运行。\n");

  return 0;
}

/**
 * 加载站点文件
 * @param inifile  输入站点的文件
 * @return
 */
bool LoadStCode(const char *inifile)
{
  CFile file;
  // 打开文件
  if (file.Open(inifile, "r") == false)
  {
    logfile.Write("站点参数文件打开失败。\n");
    return false;
  }

  char buffer[301];           // 读取一行数据的缓存
  struct st_stcode stcode;    // 读取的一整个站点数据
  CCmdStr cmdStr;             // 分割字符串

  while (1)
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
    cmdStr.GetValue(3, &stcode.lat);
    cmdStr.GetValue(4, &stcode.lon);
    cmdStr.GetValue(5, &stcode.height);

    // 写入程序容器
    vstcode.push_back(stcode);
  }

//  for (int ii = 0; ii < vstcode.size(); ii++)
//  {
//    logfile.Write("==%s-%s-%s-%.2lf-%.2lf-%.2lf==\n", \
//                  vstcode[ii].provname, vstcode[ii].obtid, vstcode[ii].obtname, \
//                  vstcode[ii].lat, vstcode[ii].lon, vstcode[ii].height);
//  }

  return true;
}

/**
 * 生成模拟的观测数据
 * 在程序内的容器之间的数据生成，不会出错
 */
void CrtSurfData()
{

  struct st_surfdata stsurfdata;
  srand(time(0));
  // 遍历站点容器
  for (int ii = 0; ii < vstcode.size(); ii++)
  {
    memset(&stsurfdata, 0, sizeof(struct st_surfdata));
    // 随机数生成观测数据
    strncpy(stsurfdata.obtid, vstcode[ii].obtid, 10);  // src超过或等于了n就不追加\0
    strncpy(stsurfdata.ddatetime, strddatetime, 14);
    stsurfdata.t = rand() % 351;       // 气温：单位，0.1摄氏度
    stsurfdata.p = rand() % 265 + 10000; // 气压：0.1百帕
    stsurfdata.u = rand() % 100 + 1;     // 相对湿度，0-100之间的值。
    stsurfdata.wd = rand() % 360;      // 风向，0-360之间的值。
    stsurfdata.wf = rand() % 150;      // 风速：单位0.1m/s
    stsurfdata.r = rand() % 16;        // 降雨量：0.1mm
    stsurfdata.vis = rand() % 5001 + 100000;  // 能见度：0.1米

    vsurfdata.push_back(stsurfdata);  // 写进容器
  }

//  for (int ii=0;ii<vsurfdata.size();ii++)
//  {
//    printf("%s %s %d-%d-%d-%d-%d-%d-%d\n", vsurfdata[ii].obtid, vsurfdata[ii].ddatetime, \
//    vsurfdata[ii].t, vsurfdata[ii].p, vsurfdata[ii].u, vsurfdata[ii].wd, \
//    vsurfdata[ii].wf, vsurfdata[ii].r, vsurfdata[ii].vis);
//  }

}

/**
 * 将观测数据写入文件中
 * @param outpath 输出文件目录，没有的话回自动新建
 * @param datafmt 输出数据的格式，支持csv，xml，json
 * @return
 */
bool CrtSurfFile(const char *outpath, const char *datafmt)
{
  char strFileName[301];
  // 拼接生成数据的文件名，例如：/tmp/idc/surfdata/SURF_ZH_20210629092200_2254.csv
  snprintf(strFileName, 300, "%s/SURF_ZH_%s_%d.%s", \
           outpath, strddatetime, getpid(), datafmt);

  // 打开文件，目录不存在回自动创建目录
  CFile file;
  // OpenForRename为了避免生成数据文件时被读到不完整的数据
  if (file.OpenForRename(strFileName, "w") == false)
  {
    logfile.Write("file.OpenForRename(%s) failed.\n", strFileName);
    return false;
  }

  // 顶部格式
  if (strcmp(datafmt, "csv") == 0) file.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");
  if (strcmp(datafmt, "xml") == 0) file.Fprintf("<data>\n");
  if (strcmp(datafmt, "json") == 0) file.Fprintf("{\"data\":[\n");

  for (int ii = 0; ii < vsurfdata.size(); ii++)
  {
    // 写入记录
    if (strcmp(datafmt, "csv") == 0)
      // "站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度
      file.Fprintf("%s,%s,%.1lf,%.1lf,%d,%d,%.1lf,%.1lf,%.1lf\n", \
                   vsurfdata[ii].obtid, vsurfdata[ii].ddatetime, vsurfdata[ii].t / 10.0, \
                   vsurfdata[ii].p / 10.0, vsurfdata[ii].u, vsurfdata[ii].wd, \
                   vsurfdata[ii].wf / 10.0, vsurfdata[ii].r / 10.0, vsurfdata[ii].vis / 10.0);

    if (strcmp(datafmt, "xml") == 0)
      file.Fprintf(
          "<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1lf</t><p>%.1lf</p><u>%d</u><wd>%d</wd><wf>%.1lf</wf><r>%.1lf</r><vis>%.1lf</vis><endl/>\n", \
                   vsurfdata[ii].obtid, vsurfdata[ii].ddatetime, vsurfdata[ii].t / 10.0, \
                   vsurfdata[ii].p / 10.0, vsurfdata[ii].u, vsurfdata[ii].wd, \
                   vsurfdata[ii].wf / 10.0, vsurfdata[ii].r / 10.0, vsurfdata[ii].vis / 10.0);

    if (strcmp(datafmt, "json") == 0)
    {
      file.Fprintf(
          "{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1lf\",\"p\":\"%.1lf\",\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1lf\",\"r\":\"%.1lf\",\"vis\":\"%.1lf\"}", \
                   vsurfdata[ii].obtid, vsurfdata[ii].ddatetime, vsurfdata[ii].t / 10.0, \
                   vsurfdata[ii].p / 10.0, vsurfdata[ii].u, vsurfdata[ii].wd, \
                   vsurfdata[ii].wf / 10.0, vsurfdata[ii].r / 10.0, vsurfdata[ii].vis / 10.0);

      if (ii < vsurfdata.size() - 1)
        file.Fprintf(",\n");
      else
        file.Fprintf("\n");
    }
  }

  if (strcmp(datafmt, "xml") == 0) file.Fprintf("</data>\n");
  if (strcmp(datafmt, "json") == 0) file.Fprintf("]}\n");

  // 关闭文件
  file.CloseAndRename();

  // 修改文件的访问atime和修改时间mtime
  UTime(strFileName, strddatetime);

  logfile.Write("生成数据文件%s，数据时间：%s，数据记录%d条。\n", strFileName, strddatetime, vsurfdata.size());

  return true;
}