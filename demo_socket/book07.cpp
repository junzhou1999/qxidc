/**
 * book05.cpp：本程序用于socket服务端的实，支持多进程连接
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

class CTcpServer{
private:
  int m_listenfd;
  int m_clientfd;
  unsigned short m_port;
  string m_clientip;

public:
  CTcpServer():m_listenfd(-1),m_clientfd(-1){}

  bool initServer(const unsigned short port);

  bool accpet();

  const string& getClientIP() const;

  bool send(const string& buffer);
  bool recv(string& buffer, int maxlen);

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

bool CTcpServer::recv(string& buffer, int maxlen)
{
  buffer.clear();            // 初始化容器
  buffer.resize(maxlen);     // 设置容器大小

  int readn;
  // :: 使用外部同名函数
  if ((readn = ::recv(m_clientfd, &buffer[0], buffer.size(), 0)) <= 0)
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
bool CTcpServer::send(const string& buffer)
{
  if (m_clientfd == -1)  return false;
  int writen;
  if ((writen = ::send(m_clientfd, buffer.data(), buffer.size(), 0)) <= 0)   // 用buffer.size而不是buffer.length
  {
    _closeClientfd();
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

// 全局变量方便exit析构
CTcpServer tcpServer;

void FathExit(int sig);  // 父进程退出处理
void ChldExit(int sig);  // 子进程退出处理

int main(int argc, char* argv[])
{
  if (argc!=2)
  {
    printf("Using: book05 port\n");
    printf("Example: ./book05 5005\n\n");
    return -1;
  }

  // 忽略所有信号的干扰，同时子进程退出（SIGCHLD）由系统处理，避免僵尸进程
  for (int ii=0;ii<64;ii++)  signal(ii, SIG_IGN);

  // 父进程的信号处理函数，支持CTRL+C，终止信号方式的退出
  signal(SIGINT, FathExit);  signal(SIGTERM, FathExit);

  if (tcpServer.initServer(atoi(argv[1])) == false)
  {
    perror("tcpServer.initServer()");
    return false;
  }

  while(true)
  {
    // 阻塞等待
    tcpServer.accpet();  cout << "客户端（" << tcpServer.getClientIP() << "）已连接。\n"; 
    // 先阻塞，再分叉
    int pid = fork();
    if (pid == -1)  {perror("fork");  return -1;}

    if (pid > 0)  
    {  // 父进程
      tcpServer._closeClientfd();
      continue;
    }

    // 子进程处理
    tcpServer._closeListenfd();
    // 子进程不接受CTRL+C
    signal(SIGINT, SIG_IGN);  signal(SIGTERM, ChldExit);  

    // 双方开始通信
    string buffer;
    while(true)  // 服务端接收来自客户端的交流请求
    {
      if (tcpServer.recv(buffer, 1024) == false)  break;  // 阻塞
      cout << "接收：" << buffer << endl;

      buffer = "ok";
      if (tcpServer.send(buffer) == false)
      {
        perror("tcpServer.send()");
        break;
      }
      cout << "发送：" << buffer << endl;
    }
    // 子进程退出，且一定要在循环后退出
    return 0;
  }  

}

// 父进程退出处理函数
void FathExit(int sig)
{
  // 防止退出处理过程被其他信号干扰
  signal(SIGINT, SIG_IGN);  signal(SIGTERM, SIG_IGN);

  cout << "父进程（" << getpid() << "）退出。";
  
  // 通知所有子进程退出
  kill(0, SIGTERM);

  // 父进程资源释放
  tcpServer._closeListenfd();

  exit(0);  // exit针对全局对象的析构，调用对象的析构函数
}

// 子进程退出处理函数
void ChldExit(int sig)
{
  // 防止退出处理过程被其他信号干扰
  signal(SIGINT, SIG_IGN);  signal(SIGTERM, SIG_IGN);

  cout << "子进程（" << getpid() << "）退出。";

  tcpServer._closeClientfd();

  exit(0);  
}
