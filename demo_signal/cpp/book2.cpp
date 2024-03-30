/*
 * 程序名：book2.cpp 本程序用于给共享内存加上简单的二元锁
 * 时间：2024-01-10 13:56
*/
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>

class csem
{
private:
  union semun
  {
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
  };

  int m_semid = -1;

public:
  // 如果信号量已存在，获取信号量；如果信号量不存在，则创建它并初始化为value。
  bool init(key_t key);

  bool P(short sem_op = -1); // 信号量的P操作。
  bool V(short sem_op = 1);  // 信号量的V操作。
  int value();            // 获取信号量的值，成功返回信号量的值，失败返回-1。
//  bool destroy();          // 销毁信号量。
  ~csem();
};

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

  csem sem;
  sem.init(key_t(0x5025));

  // 程序连接共享内存
  char *ptext = 0;
  ptext = (char *) shmat(shmid, 0, 0);

  // 读取共享内存
  printf("aaa 操作前的共享内存内容：%s\n", ptext);
  sem.P();  // 上锁

  sleep(6);  // 握有锁一段时间，使其他程序阻塞等待锁
  printf("aaa 操作前的共享内存内容：%s\n", ptext);

  // 修改共享内存
  strncpy(ptext, argv[1], 1024);

  // 读取修改后的共享内存
  printf("操作后的共享内存内容：%s\n", ptext);
  sem.V();  // 解锁
  printf("操作后的共享内存内容：%s\n", ptext);

  // 断连共享内存
  shmdt(ptext);

  return 0;
}

bool csem::init(key_t key)
{
  // 如果信号量已存在，则不再新建
  if (m_semid != -1) return false;

  // 信号量的初始化不能直接用semget(key,1,0666|IPC_CREAT)，因为信号量创建后，初始值是0。
  // 如果直接设置资源的值，那么如果是获取到的而不是创建来的信号量，会直接把锁给到正在等待的用户

  // 信号量的初始化分三个步骤：
  // 1）获取信号量，如果成功，函数返回。
  // 2）如果失败，则创建信号量。
  // 3) 设置信号量的初始值。

  // 获取信号量，这里只获取不创建
  if ((m_semid = semget(key, 1, 0666)) == -1)
  {
//sleep(5);  如果两个程序走到这里表示获取不了信号量，那么以下代码就可以分开谁创建谁获取了
    // 如果信号量不存在，创建它。
    if (errno == 2)
    {
      // 用IPC_EXCL标志确保只有一个进程创建并初始化信号量，其它进程只能获取。
      if ((m_semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
      {
        if (errno != EEXIST)
        {
          perror("init 1 semget()");
          return false;
        }
        if ((m_semid = semget(key, 1, 0666)) == -1)
        {
          perror("init 2 semget()");
          return false;
        }
        // 走到这里是为了避免多个进程创建相同key的信号量，这里是获取信号量的进程的终点
        return true;  // EEXIST, semget(获取不创建)

      }

      // 信号量创建成功后，还需要把它初始化成value。
      union semun sem_union;
      sem_union.val = 1;   // 设置信号量的初始值，二值信号量填1
      if (semctl(m_semid, 0, SETVAL, sem_union) < 0)
      {
        perror("init semctl()");
        return false;
      }
    }
    else  // 获取信号量得到其他错误，返回
    {
      perror("init 3 semget()");
      return false;
    }
  }

  return true;
}

// 如果把sem_flg设置为SEM_UNDO，操作系统将跟踪进程对信号量的修改情况，
// 在全部修改过信号量的进程（正常或异常）终止后，操作系统将对信号量
// 恢复为初始值（就像撤销了全部进程对信号的操作）。
// 如果信号量用于互斥锁，设置为SEM_UNDO
// 如果信号量用于生成消费者模型，设置为0
bool csem::P(short sem_op)
{
  if (m_semid == -1) return false;

  struct sembuf sem_b;
  sem_b.sem_num = 0;      // 信号量编号，0代表第一个信号量。
  sem_b.sem_op = sem_op;  // P操作的sem_op必须小于0。
  sem_b.sem_flg = SEM_UNDO;
  if (semop(m_semid, &sem_b, 1) == -1)
  {
    perror("p semop()");
    return false;
  }

  return true;
}

/**
 * 释放资源
 * @param sem_op
 * @return
 */
bool csem::V(short sem_op)
{
  if (m_semid == -1) return false;

  struct sembuf sem_b;
  sem_b.sem_num = 0;      // 信号量编号，0代表第一个信号量。
  sem_b.sem_op = sem_op;  // P操作的sem_op必须小于0。
  sem_b.sem_flg = SEM_UNDO;
  if (semop(m_semid, &sem_b, 1) == -1)
  {
    perror("p semop()");
    return false;
  }

  return true;
}

// 获取信号量的值，成功返回信号量的值，失败返回-1。
int csem::value()
{
  return semctl(m_semid, 0, GETVAL);
}

csem::~csem()
{}



