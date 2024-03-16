/*
 * 程序名：ftpgetfiles.cpp 本程序用于将ftp服务器的文件下载下来
 * 时间：2024-03-12 18:38
*/

#include "_public.h"
#include "_ftp.h"

CLogFile logfile;

// 程序主函数
bool _ftpgetfiles();

Cftp ftp;  // ftp服务类

CDir dir;  // 文件服务类

struct st_arg
{
  char hostname[31];        // ftp登录的服务器地址。
  char username[31];        // ftp登录用户名。
  char password[31];        // ftp登录密码。
  unsigned int mode;        // ftp传输模式，1：被动模式；2-主动模式，缺省别动模式。
  char localpath[301];      // 本地下载的文件目录。
  char remotepath[301];     // 远程登录的文件目录。
  char matchname[101];      // 在远程目录匹配需要下载的文件名。
  char listfilename[301];   // 存储下载前列出服务器的文件清单的文件名
//  char okfilename[301];     // 存储在本地的本次ftp下载信息，每次下载会追加文件内容，ptype为1生效。
  unsigned int ptype;       // 传输完成后服务器对文件的操作，1：什么也不做；2：删除；3-拷贝备份到remotepathbak目录。
  char remotepathbak[301];  // ftp服务器的文件备份目录
} starg;

// xml参数解析
bool _xml2arg(const char *argxml);

struct st_fileinfo
{
  char filename[301];
  char mtime[21];
};

vector<struct st_fileinfo> vfilelist;

// 程序帮助文档
void _help();

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    _help();
    return -1;
  }

  // 屏蔽掉信号和IO
  CloseIOAndSignal(true);
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  // 打开日志
  if (logfile.Open(argv[1]) == false)
  {
    printf("logfile.Open(%s) failed.\n", argv[1]);
    return -1;
  }
  // 解析xml
  if (_xml2arg(argv[2]) == false)
  {
    logfile.Write("xml \"%s\" analysis failed.\n", argv[2]);
    return -1;
  }

  // 登录ftp
  if (ftp.login(starg.hostname, starg.username, starg.password, starg.mode) == false)
  {
    logfile.Write("ftp.login(%s,%s,%s,%s) failed.\n", starg.hostname, starg.username, starg.password,
                  starg.mode == 1 ? "Passive" : "Active");
    return -1;
  }

  _ftpgetfiles();

  return 0;
}

// 程序主函数
bool _ftpgetfiles()
{
  // 打开远程主机目录
  if (ftp.chdir(starg.remotepath) == false)
  {
    logfile.Write("ftp.chdir(%s) failed.\n", starg.remotepath);
    return false;
  }

  // 读取文件列表，listfilename需要在本地有权限
  if (ftp.nlist(".", starg.listfilename) == false)
  {
    logfile.Write("ftp.nlist(%s) failed.\n", starg.remotepath);
    return false;
  }

  // 读取文件列表
  CFile file;
  if (file.Open(starg.listfilename, "r") == false)
  {
    logfile.Write("file.Open(%s) failed.\n", starg.listfilename);
    return false;
  }

  struct st_fileinfo fileinfo;
  while (true)
  {
    memset(&fileinfo, 0, sizeof(struct st_fileinfo));

    // 读取文件名
    if (file.Fgets(fileinfo.filename, 300, true) == false) break;

    // 筛选
    if (MatchStr(fileinfo.filename, starg.matchname) == false) continue;

    vfilelist.push_back(fileinfo);
  }

  // 拉取文件
  char strremotefilename[301], strlocalfilename[301];
  for (int ii = 0; ii < vfilelist.size(); ii++)
  {
    SNPRINTF(strremotefilename, sizeof(strremotefilename), 300, "%s/%s", starg.remotepath, vfilelist[ii].filename);
    SNPRINTF(strlocalfilename, sizeof(strremotefilename), 300, "%s/%s", starg.localpath, vfilelist[ii].filename);
    logfile.Write("get %s ... ", strremotefilename);
    if (ftp.get(strremotefilename, strlocalfilename) == false)
    {
      logfile.WriteEx("failed.\n");
      continue;
    }
    logfile.WriteEx("ok.\n");

    // 删除ftp服务器里刚下载的文件
    if (starg.ptype == 2)
    {
      if (ftp.ftpdelete(strremotefilename) == false)
      {
        logfile.Write("ftp.ftpdelete(%s) failed.\n", strremotefilename);
        return false;
      }
    }

    // 备份ftp服务器里刚下载的文件
    if (starg.ptype == 3)
    {
      char strremotebakfilename[301];
      SNPRINTF(strremotebakfilename, sizeof(strremotebakfilename), 300, "%s/%s", starg.remotepathbak,
               vfilelist[ii].filename);

      if (ftp.ftprename(strremotefilename, strremotebakfilename) == false)
      {
        logfile.Write("ftp.ftprename(%s to %s) failed.\n", strremotebakfilename, strremotebakfilename);
        return false;
      }
    }
  }

}

// 程序帮助文档
void _help()
{
  printf("\n");
  printf("Using: ftpgetfiles logfile ftpxml\n");

  printf(
      "Example:/project/tools/bin/ftpgetfiles /logs/tools/ftpgetfiles.log \"<host>192.168.0.83</host><user>abc</user><password>123</password><mode>1</mode><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.CSV,SURF_ZH*.XML</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak>\"\n\n");

  printf("本程序用于将ftp服务器的文件下载到本地文件夹，共需要两个参数：\n");
  printf("logfile：日志文件的位置。\n");
  printf("ftpxml：ftp操作所需要的相关XML参数，主要包括：\n");
  printf("<host>val</host>：ftp服务的地址。\n");
  printf("<user>val</user>：连接ftp服务器的用户名。\n");
  printf("<password>val</password>：连接ftp服务器的用户名密码。\n");
  printf("<localpath>val</localpath>：本地存放文件的目录。（目录需要存在）\n");
  printf("<remotepath>val</remotepath>：远程ftp服务器文件的目录。（目录需要存在）\n");
  printf("<matchname>val</matchname>：匹配远程ftp服务器目录的文件格式。\n");
  printf("<listfilename>val</listfilename>：下载前列出服务器文件夹所有的文件名的文件。\n");
//  printf("<okfilename>val</okfilename>：存储下载好的文件列表的本地文件路径。\n");
  printf("<ptype>val</ptype>：传输完成后服务器对文件的操作，1：什么也不做；2：删除；3-拷贝备份到remotepathbak目录。\n");
  printf(
      "<remotepathbak>val</remotepathbak>：这是一个可选参数，用于服务器传输完成文件的备份，只有ptype为3时才生效且必须配置。（目录需要存在）\n\n");

}

bool _xml2arg(const char *arg)
{
  memset(&starg, 0, sizeof(struct st_arg));

  GetXMLBuffer(arg, "host", starg.hostname, 30);
  if (strlen(starg.hostname) == 0)
  {
    logfile.Write("host is null.\n");
    return false;
  }
  GetXMLBuffer(arg, "user", starg.username, 30);
  if (strlen(starg.username) == 0)
  {
    logfile.Write("user is null.\n");
    return false;
  }
  GetXMLBuffer(arg, "password", starg.password, 30);
  if (strlen(starg.password) == 0)
  {
    logfile.Write("password is null.\n");
    return false;
  }
  GetXMLBuffer(arg, "mode", &starg.mode);
  if (starg.mode != 2) starg.mode = 1;
  GetXMLBuffer(arg, "localpath", starg.localpath, 300);
  if (strlen(starg.localpath) == 0)
  {
    logfile.Write("localpath is null.\n");
    return false;
  }
  GetXMLBuffer(arg, "remotepath", starg.remotepath, 300);
  if (strlen(starg.remotepath) == 0)
  {
    logfile.Write("remotepath is null.\n");
    return false;
  }
  GetXMLBuffer(arg, "matchname", starg.matchname, 100);
  if (strlen(starg.matchname) == 0)
  {
    logfile.Write("matchname is null.\n");
    return false;
  }
  GetXMLBuffer(arg, "listfilename", starg.listfilename, 300);
  if (strlen(starg.listfilename) == 0)
  {
    logfile.Write("listfilename is null.\n");
    return false;
  }
//  GetXMLBuffer(arg, "okfilename", starg.okfilename, 300);
//  if (strlen(starg.okfilename) == 0)
//  {
//    logfile.Write("okfilename is null.\n");
//    return false;
//  }
  GetXMLBuffer(arg, "ptype", &starg.ptype);
  if (starg.ptype != 1 && starg.ptype != 2 && starg.ptype != 3)
  {
    logfile.Write("ptype error.\n");
    return false;
  }

  GetXMLBuffer(arg, "remotepathbak", starg.remotepathbak, 300);
  if (starg.ptype == 3 && strlen(starg.remotepathbak) == 0)
  {
    logfile.Write("remotepathbak is null.\n");
    return false;
  }


  return true;
}