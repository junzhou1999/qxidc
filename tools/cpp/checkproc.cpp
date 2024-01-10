/*
 * 程序名：checkproc.cpp 本程序用于管理守护共享内存中的进程
 * 时间：2024-01-10 16:10
*/

#include "_public.h"

CLogFile logfile;

int main(int argc, char *argv[])
{
  // 程序帮助文档
  if (argc != 2)
  {
    printf("Using:checkproc logfile\n");
    printf("Example:/project/tools/bin/procctl 10 /project/tools/bin/checkproc /logs/tools/checkproc.log\n\n");

    printf("本程序用于检查后台服务程序是否超时，如果已超时，就终止它。\n");
    printf("注意：\n");
    printf("  1）本程序由procctl启动，运行周期建议为10秒。\n");
    printf("  2）为了避免被普通用户误杀，本程序应该用root用户启动。\n");
    printf("  3）如果要停止本程序，只能用killall -9 终止。\n\n\n");
    return -1;
  }

  // 关闭信号干扰和IO
  CloseIOAndSignal(true);

  // 要先关闭所有的文件输出，再打开日志的文件输出
  if ((logfile.Open(argv[1], "a+")) == false)
  {
    printf("logfile.Open(%s) failed.\n", argv[1]);
    return -1;
  }

  // 创建/连接共享内存
  int shmid = 0;
  if ((shmid = shmget(SHMKEYP, sizeof(struct st_procinfo) * MAXNUMP, 0666 | IPC_CREAT)) == -1)
  {
    logfile.Write("shmget(%x) failed.\n", SHMKEYP);
    return -1;
  }

  struct st_procinfo *shm = (struct st_procinfo *) shmat(shmid, 0, 0);

  // 遍历共享内存
  for (int ii = 0; ii < MAXNUMP; ii++)
  {
    // 如果pid为空，continue
    if ((shm + ii)->pid == 0) continue;

//    logfile.Write("aaa 进程%d(%s),atime(%ld),timeout(%ld)。\n", (shm + ii)->pid, (shm + ii)->pname, (shm + ii)->atime,
//                  (shm + ii)->timeout);
//    continue;


    // 发送信号0，判断程序是否还在
    int iret = kill((shm + ii)->pid, 0);
    if (iret == -1)  // -1，不存在
    {
      logfile.Write("进程pid=%d(%s)已不存在。\n", (shm + ii)->pid, (shm + ii)->pname);
      memset(shm + ii, 0, sizeof(struct st_procinfo));
      continue;
    }

    // 如果程序未超时，continue
    time_t tnow = time(0);
    if (tnow - (shm + ii)->atime < (shm + ii)->timeout) continue;

    // 如果程序超时，尝试用信号15终止它
    logfile.Write("进程pid=%d(%s)已超时。\n", (shm + ii)->pid, (shm + ii)->pname);
    kill((shm + ii)->pid, SIGTERM);

    // 停留5s判断进程是否还活着
    for (int jj = 0; jj < 5; jj++)
    {
      sleep(1);
      iret = kill((shm + ii)->pid, 0);
      if (iret == -1) break;  // 进程已不存在
    }

    // 用信号9强行终止它
    if (iret == -1)
    {
      logfile.Write("进程pid=%d(%s)已正常终止。\n", (shm + ii)->pid, (shm + ii)->pname);
    }
    else
    {
      kill((shm + ii)->pid, SIGKILL);
      logfile.Write("进程pid=%d(%s)已强制终止。\n", (shm + ii)->pid, (shm + ii)->pname);
    }

    // 关闭超时进程所占用的内存
    memset(shm + ii, 0, sizeof(struct st_procinfo));
  }
  // 断开共享内存
  shmdt(shm);

  return 0;
}

