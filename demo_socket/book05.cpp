/**
 * book03.cpp：本程序用于socket服务端的实现
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

class CTcpServer{
private:
  int m_listenfd;
  int m_clientfd;
  unsigned short m_port;
  string m_clientip;
  bool _closeListenfd();
  bool _closeClientfd();

public:
  CTcpServer():m_listenfd(-1),m_clientfd(-1){}

  bool initServer(const unsigned short port);

  bool accpet();

  const string& getClientIP() const;

  bool send(const string& buffer);
  bool recv(string& buffer, int maxlen);

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
   return -1; 
  }

  // sockets设置为监听模式，参数3为可以容纳的established队列个数-1
  if (listen(m_listenfd, 3) != 0 )
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

int main(int argc, char* argv[])
{
  if (argc!=2)
  {
    printf("Using: book03 port\n");
    printf("Example: ./book03 5005\n\n");
    return -1;
  }

  CTcpServer tcpServer;
  if (tcpServer.initServer(atoi(argv[1])) == false)
  {
    perror("tcpServer.initServer()");
    return false;
  }
  tcpServer.accpet();  cout << "客户端（" << tcpServer.getClientIP() << "）已连接。\n"; 


  // 双方开始通信
  string buffer;
  while(true)  // 服务端接收来自客户端的交流请求
  {
    if (tcpServer.recv(buffer, 1024) == false)  break;
    cout << "接收：" << buffer << endl;

    buffer = "ok";
    if (tcpServer.send(buffer) == false)
    {
      perror("tcpServer.send()");
      break;
    }
    cout << "发送：" << buffer << endl;
  }

  return 0;
}

