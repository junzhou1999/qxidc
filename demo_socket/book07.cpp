/**
 * book07.cpp：本程序用于socket服务端的实现，支持多线程连接
*/
#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
using namespace std;
#include <signal.h>
#include <pthread.h>
#include <list>

class CTcpServer{
private:
  int m_listenfd;  // 进程（线程）的监听socket
  int m_clientfd;  // 最新连接上来的客户端socket
  unsigned short m_port;  // 服务端端口
  string m_clientip;      // 连接上来客户端的IP地址，表示：点分十进制

public:
  CTcpServer():m_listenfd(-1),m_clientfd(-1){}

  bool initServer(const unsigned short port);

  bool accpet();

  const string& getClientIP() const;

  const int& getClientfd() const;

  bool send(const string& buffer){}
  bool recv(string& buffer, int maxlen){}

  bool _closeListenfd();
  bool _closeClientfd();

  ~CTcpServer(){_closeClientfd(); _closeListenfd();}
};


bool CTcpServer::initServer(const unsigned short in_port)
{
  // 创建socket
  if ((m_listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    return false;
  }
  m_port = in_port;

  // 配置连接参数，绑定
  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  // serveraddr.sin_addr.s_addr = inet_addr("118.89.50.198"); // 指定ip地址。
  serveraddr.sin_port = htons(m_port);
  // 支持bind已有连接的端口，避免服务端主动端口后在TCP挥手的时候需要等待2MSL时间内重新打开服务
  int opt = 1; unsigned int len = sizeof(opt);
  setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,len);    
  if (bind(m_listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) !=0 )
  {
   perror("bind");
   return false; 
  }

  // sockets设置为监听模式，参数3为可以容纳的established队列个数-1
  // 改成5把，多支持几个客户端连接队列
  if (listen(m_listenfd, 5) != 0 )
  {
    perror("listen");
    return -1;
  }
  
  return true;
}

bool CTcpServer::accpet()
{
  struct sockaddr_in clientaddr;
  int addrlen = sizeof(clientaddr);
  // 在这里阻塞等待
  m_clientfd = accept(m_listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&addrlen);

  m_clientip = inet_ntoa(clientaddr.sin_addr);  // 大端序的IP转成点分十进制的IP

  return true;
}

const string& CTcpServer::getClientIP() const
{
  return this->m_clientip; 
}

const int& CTcpServer::getClientfd() const
{
  return this->m_clientfd; 
}

/***
 * connfd：要通信的socket端口
 * buffer：string类buffer缓存
 * maxlen：自定义设置buffer的大小，应该考虑报文长度
*/
bool Recv(const int connfd, string& buffer, int maxlen)
{
  buffer.clear();            // 初始化容器
  buffer.resize(maxlen);     // 设置容器大小

  int readn;
  if ((readn = recv(connfd, &buffer[0], buffer.size(), 0)) <= 0)
  {
    buffer.clear();
    return false;
  }

  buffer.resize(readn);  // 动态改变string容器
  return true;
}

/**
 * 常引用的string容器可以接收string对象和c语言字符串
*/
bool Send(const int connfd, const string& buffer)
{
  if (connfd == -1)  return false;
  int writen;
  if ((writen = send(connfd, buffer.data(), buffer.size(), 0)) <= 0)   // 用buffer.size而不是buffer.length
  {
    // close(connfd);
    return false;
  }

  return true;
}

bool CTcpServer::_closeListenfd()
{
  if (m_listenfd != -1)  close(m_listenfd);
  m_listenfd = -1;
  return true;
}

bool CTcpServer::_closeClientfd()
{
  if (m_clientfd != -1)  close(m_clientfd);
  m_clientfd = -1;
  return true;
}

// 线程主函数，负责服务端的通信
void* thmain(void* arg);

pthread_spinlock_t lsSpinLock;  // 给共享的链表容器加的自旋锁
list<pthread_t> lspthid;        // 保存线程id的全局链表，用于线程退出时处理

// 线程退出处理
void thcleanup(void* arg);

// 全局变量方便exit析构
CTcpServer tcpServer;

void Exit(int sig);  // 进程信号退出处理

int main(int argc, char* argv[])
{
  if (argc!=2)
  {
    printf("Using: book07 port\n");
    printf("Example: ./book07 5005\n\n");
    return -1;
  }

  // 忽略所有信号的干扰，同时子进程退出（SIGCHLD）由系统处理，避免僵尸进程
  for (int ii=0;ii<64;ii++)  signal(ii, SIG_IGN);

  // 父进程的信号处理函数，支持CTRL+C，终止信号方式的退出
  signal(SIGINT, Exit);  signal(SIGTERM, Exit);

  if (tcpServer.initServer(atoi(argv[1])) == false)
  {
    perror("tcpServer.initServer()");
    return false;
  }

  // 自旋锁的初始化
  pthread_spin_init(&lsSpinLock, 0);

  while(true)
  {
    // 阻塞等待
    tcpServer.accpet();  cout << "客户端（" << tcpServer.getClientIP() << "）已连接。\n"; 
    // 先阻塞，再分叉
    pthread_t thmid;
    if (pthread_create(&thmid, NULL, thmain, (void*)(long)tcpServer.getClientfd()) != 0)
    {
      perror("pthread_create");
      return -1;
    }

    pthread_spin_lock(&lsSpinLock);    // 加锁

    // 更新维护队列
    lspthid.push_back(thmid);

    pthread_spin_unlock(&lsSpinLock);  // 释放锁
  }  

  //return 0;
}

// 线程主函数，负责服务端的通信
void* thmain(void* arg)
{
  pthread_cleanup_push(thcleanup, arg);  // 线程资源清理函数入栈

  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);  // 线程的取消方式为立即取消

  pthread_detach(pthread_self());        // 线程分离

  int connfd = (int)(long)arg;           // 需要保留新连上了客户端socket

  string buffer;
  while(true)   
  {
    // 不能再用类里面的传输了，因为客户端连接传输socket在线程里只有一份
    if (Recv(connfd, buffer, 1024) == false)  break;  // 阻塞
    cout << "接收：" << buffer << "readn=" << buffer.size() << "。\n";

    buffer = "ok";
    if (Send(connfd, buffer) == false)
    {
      perror("Send()");
      break;
    }
    cout << "发送：" << buffer << endl;
  }

  pthread_spin_lock(&lsSpinLock);
  // 线程结束的处理
  // 维护链表
  for (list<pthread_t>::iterator it=lspthid.begin(); it!=lspthid.end();it++) 
  {
    if (pthread_equal(*it, pthread_self()) !=0 )  
    {
      lspthid.erase(it);  // 如果删除完之后还要迭代erase(it++)
      break;
    }
  }

  pthread_spin_unlock(&lsSpinLock);
  
  pthread_cleanup_pop(1); // 线程清理函数出栈，close(connfd)

  return 0;
}

// 线程清理是清理资源
// 不同于线程结束的处理
void thcleanup(void* arg)
{
  close((long)arg);
  cout << "线程（" << pthread_self() << ")已被清理。\n";
}

// 进程退出处理函数
void Exit(int sig)
{
  // 防止退出处理过程被其他信号干扰
  signal(SIGINT, SIG_IGN);  signal(SIGTERM, SIG_IGN);

  cout << "进程退出，sig=" << sig << ".\n";
  
  // 父进程资源释放
  tcpServer._closeListenfd();
  
  cout << "lspthid.size():" << lspthid.size() << endl;
  pthread_spin_lock(&lsSpinLock);
  // 取消全部的线程
  for (list<pthread_t>::iterator it=lspthid.begin(); it!=lspthid.end(); it++)
  {
    pthread_cancel(*it);
  }
  pthread_spin_unlock(&lsSpinLock);
  sleep(1);  // 执行线程清理需要时间

  pthread_spin_destroy(&lsSpinLock);

  exit(0);  // exit针对全局对象的析构，调用对象的析构函数
}
