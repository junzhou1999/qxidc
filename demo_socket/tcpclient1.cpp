/**
 * tcpclient1.cpp：本程序用于socket客户端的实现，测试服务端的复用特性，属于非阻塞连接
*/
#include <stdio.h>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
using namespace std;

class CTcpClient
{
private:
  int m_sockfd;
  bool setNoBlock();
  bool _close();
public:
  CTcpClient():m_sockfd(-1){}

  bool connect(const char* ip, const unsigned short port);

  bool send(const string& buffer);
  bool recv(string& buffer, int maxlen);

  ~CTcpClient(){_close();}

};

bool CTcpClient::_close()
{
  if (m_sockfd != -1)  close(m_sockfd);
  m_sockfd = -1;
  return true;
}

/**
 *  客户端连接服务端实现 
*/
bool CTcpClient::connect(const char* in_ip, const unsigned short in_port)
{
  // 创建socket
  if (m_sockfd != -1)  return false;

  if ((m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    return false;
  }
  // 在获取socket后设置标志
  this->setNoBlock();

  // 配置服务器信息，连接
  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  int addrlen = sizeof(serveraddr);
  serveraddr.sin_family = AF_INET;        // 协议簇
  serveraddr.sin_port = htons(in_port);   // 端口
  struct hostent* h = NULL;
  if (( h = gethostbyname(in_ip)) == NULL)
  {
    cout << "gethostbyname(" << in_ip <<  "failed.\n";
    _close();
    return false;
  }
  memcpy(&serveraddr.sin_addr, h->h_addr, h->h_length);  // IP地址

  // 主动连接
  if (::connect(m_sockfd, (struct sockaddr*)&serveraddr, (socklen_t)addrlen) != 0)
  {
    // 调用非阻塞的socket，无论是否连接成功，它都是失败的
    if(errno != EINPROGRESS)    
    {
      perror("connect");
      _close();
      return false;
    }
    // 因为阻塞而产生的连接错误可以忽略 
    // cout << "errno=" << errno << "，EINPROGRESS=" << EINPROGRESS << endl;
  }

  // 非阻塞的socket，如果连接是可写的，那么连接是成功的
  struct pollfd fds;
  fds.fd = m_sockfd;
  fds.events = POLLOUT;
  poll(&fds, 1, -1);
  if (fds.revents != POLLOUT)
  {
    cout << "connect failed.\n";
    return false;
  }

  cout << "connect ok.\n";
  return true;
}

bool CTcpClient::send(const string& buffer)
{
  if (m_sockfd == -1)  return false;
  int writen;
  // 只发送有效字符串，不发送整个容器
  if ((writen = ::send(m_sockfd, buffer.data(), strlen(buffer.data()), 0)) <= 0)
  {
    perror("send");
    // _close();
    return false;
  }

  return true;
}

bool CTcpClient::recv(string& buffer, int maxlen)
{
  buffer.clear();
  buffer.resize(maxlen);  // 动态更改buffer容器
  int readn;
  if ( (readn = ::recv(m_sockfd, &buffer[0], maxlen, 0)) < 0)  return false;

  buffer.resize(readn);
  return true;

}

/**
 * @brief 设置socket为非阻塞socket
 * fcntl()函数当获取socket的标志位时，返回值是其标志位 
 * @return true 
 * @return false 
 */
bool CTcpClient::setNoBlock()
{
  if (m_sockfd == -1)  return false;

  int flags;
  if ( (flags=fcntl(m_sockfd, F_GETFL, 0)) == -1)   flags = 0;

  fcntl(m_sockfd, F_SETFL, flags|O_NONBLOCK);
  
  return true;
}

int main(int argc, char* argv[])
{
  if (argc!=3)
  {
    cout << "Using: tcpclient ip port\n";
    cout << "Example: ./tcpclient 192.168.0.74 5005\n\n";
    return -1;
  }

  CTcpClient tcpClient;
  if (tcpClient.connect(argv[1], atoi(argv[2])) == false)
  {
    perror("tcpClient.connect()");
    return -1;
  }

  // 通信
  string buffer;
  while (true)
  {
    cout << "input connection message(type quit to exit):\n";
    cin >> buffer;

    if (buffer == "quit")  break;
    if ( (tcpClient.send(buffer)) == false)
    {
      perror("tcpClient.send()");
      return false;
    }
    cout << "send message (" << buffer <<  ") ok.\n";  

    if ( (tcpClient.recv(buffer, 1024)) == false)  break;
    cout << "接收：" << buffer << endl;
  }

  // 析构资源
  return 0;
}
