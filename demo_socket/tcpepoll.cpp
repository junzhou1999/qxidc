/**
 * tcppoll.cpp：本程序用于socket服务端的实现，支持poll特性的IO复用技术
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
#include <poll.h>

// 初始化listen socket，传参
bool initServer(int& listenfd, const unsigned short in_port);

// 监听端口接受客户端的连接
bool Accept(const int& listenfd, int& clientfd, struct sockaddr_in& clientaddr);

int main(int argc, char* argv[])
{
  if (argc!=2)
  {
    cout << "Using: tcppoll port\n";
    cout << "Example: ./tcppoll 5005\n";
    cout << "./tcpclient 127.0.0.1 5005\n\n";

    return -1;
  }

  int listenfd;  // 监听socket

  // 初始化监听端口
  if ( initServer(listenfd, atoi(argv[1])) == false)
  {
    perror("initServer()");
    return -1;
  }

  struct pollfd fds[1024];    // 声明poll结构体
  // 0  1  2  3  .. 1023
  // -1 -1 -1 -1 .. -1

  // 初始化
  for (int ii=0; ii<1024; ii++)  fds[ii].fd = -1;

  // 打算让poll监听的事件
  fds[listenfd].fd = listenfd;
  fds[listenfd].events = POLLIN;           // POLLIN表示就监听读事件，POLLOUT表示写事件
  // fds[listenfd].events = POLLIN|POLLOUT;   // 都监听

  int maxfd = listenfd;        // 初始化取最大的临时socket

  cout << "服务端（listen_socket:" << listenfd << "）就绪。\n";

  // 外层循环，一直select，监视socket
  while(true)
  {
    /* 
     * 阻塞监听事件，有事件响应poll()函数就返回
     * ifds：事件变化的个数或者监视结果
     * 1.poll结构体
     * 2.最大的fd
     * 3.超时时间（毫秒ms）：100秒
    */
    int infds = poll(fds, maxfd+1, 100000);
    if (infds < 0)    // 函数调用失败
    {
      perror("poll() failed");
      break;
    }

    if (infds == 0)   // 超时
    {
      perror("poll() timeout");
      continue;
    }

    // 读事件变化
    // 遍历所有在poll监视中的socket？注意：是从0遍历到最大的那个socket，所以要用<=
    for (int eventfd=0; eventfd<=maxfd; eventfd++)
    {
      // 这两个continue必须存在，需要及时处理连接和通信

      // 只需要定位有事件发生的socket
      if ( fds[eventfd].fd < 0)  continue;  

      // 只需要定位读事件
      if ((fds[eventfd].revents&POLLIN) == 0)  continue;

      // 监听事件
      if (eventfd == listenfd)
      {
        // 有客户端连上来
        int clientfd;  struct sockaddr_in clientaddr;
        if (Accept(eventfd, clientfd, clientaddr) == false)
        {
          perror("Accpet()");
          continue;
        }
        // cout << "客户端（" << inet_ntoa(clientaddr.sin_addr) <<  "）已连接。" << endl;  // 大端序的IP转成点分十进制的IP
        cout << "客户端（socket:" << clientfd <<  "）已连接。" << endl;

        // socket加入的操作
        fds[clientfd].fd = clientfd;
        fds[clientfd].events = POLLIN;
        if (clientfd > maxfd)  maxfd = clientfd;
      }
      else 
      // 通信事件，接收报文失败
      {
        string buffer;  buffer.clear();  buffer.resize(1024);

        if (recv(eventfd, &buffer[0], buffer.size(), 0) <= 0)
        {
          // 通信断开的一些操作
          cout << "客户端（socket:" << eventfd << "）已断开。\n"; 

          fds[eventfd].fd = -1;
          close(eventfd);

          for (int ii=maxfd; ii>0; ii--)   // 重新找最大的fd
          {
            // 找到一个最大的
            if (fds[ii].fd != -1)  
            {
              maxfd = ii;
              break;
            }
          }
        }
        else
        // 通信事件，接收报文成功
        {
          cout << "接收：" << buffer << endl;

          buffer = "ok";
          if (send(eventfd, buffer.data(), strlen(buffer.data()), 0) <= 0)
          {
            cout << "客户端（socket:" << eventfd << "）已断开。\n"; 

            fds[eventfd].fd = -1;
            close(eventfd);

            // 找到一个最大的
            for (int ii=maxfd; ii>0; ii--)   // 重新找最大的fd
            {
              // 找到一个最大的
              if (fds[ii].fd != -1)  
              {
                maxfd = ii;
                break;
              }
            }
          }
          cout << "发送：" << buffer << endl; 
        }
      }   // 监听or通信
    }     // 遍历poll结构体数组
  }       // 循环poll

  return 0;
}

// 初始化listen socket，传参
bool initServer(int& listenfd, const unsigned short in_port)
{
    // 创建socket
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    return false;
  }

  // 配置连接参数，绑定
  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  // serveraddr.sin_addr.s_addr = inet_addr("118.89.50.198"); // 指定ip地址。
  serveraddr.sin_port = htons(in_port);
  // 支持bind已有连接的端口，避免服务端主动端口后在TCP挥手的时候需要等待2MSL时间内重新打开服务
  int opt = 1; unsigned int len = sizeof(opt);
  setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,len);    
  if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) !=0 )
  {
   perror("bind");
   return false; 
  }

  // sockets设置为监听模式，参数为可以容纳的established队列个数-1
  if (listen(listenfd, 5) != 0 )
  {
    perror("listen");
    return false;
  }
  
  return true;
}

/**
 * 监听端口接受客户端的连接
*/
bool Accept(const int& listenfd, int& clientfd, struct sockaddr_in& clientaddr)
{
  int addrlen = sizeof(clientaddr);
  // 在这里阻塞等待
  clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&addrlen);

  return true;
}

