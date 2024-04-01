/**
 * book08.cpp：本程序用于socket客户端的实现，测试服务端的多线程支持
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
using namespace std;

class CTcpClient
{
private:
  int m_sockfd;
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

  // 配置服务器信息，连接
  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  int addrlen = sizeof(serveraddr);
  serveraddr.sin_family = AF_INET;        // 协议簇
  serveraddr.sin_port = htons(in_port);   // 端口
  struct hostent* h = NULL;
  if (( h = gethostbyname(in_ip)) == NULL)
  {
    printf("gethostbyname(%s) failed.\n", in_ip);
    _close();
    return false;
  }
  memcpy(&serveraddr.sin_addr, h->h_addr, h->h_length);  // IP地址

  // 主动连接
  if (::connect(m_sockfd, (struct sockaddr*)&serveraddr, (socklen_t)addrlen) != 0)
  {
    perror("connect");
    _close();
    return false;
  }

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

int main(int argc, char* argv[])
{
  if (argc!=3)
  {
    printf("Using: book08 ip port\n");
    printf("Example: ./book08 192.168.0.74 5005\n\n");
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
  for (int ii=0; ii<100; ii++)
  {
    buffer.resize(1024);
    sprintf(&buffer[0], "这是第%d个超女，编号：%010d。", ii, ii+1);

    if ( (tcpClient.send(buffer)) == false)
    {
      perror("tcpClient.send()");
      return false;
    }
    cout << "发送：" << buffer << endl;  

sleep(1);

    if ( (tcpClient.recv(buffer, 1024)) == false)
    {
      break;
    }
    cout << "接收：" << buffer << endl;
  }

  // 析构资源
  return 0;
}
