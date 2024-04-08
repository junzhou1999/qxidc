/**
 * tcpselect.cpp：本程序用于socket服务端的实现，支持select特性的IO复用技术
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
#include <sys/select.h>

// 初始化listen socket，传参
bool initServer(int& listenfd, const unsigned short in_port);

// 监听端口接受客户端的连接
bool Accept(const int& listenfd, int& clientfd, struct sockaddr_in& clientaddr);

/**
 * @brief 
 * maxfd：最大的位图socket，用于遍历位图定位
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[])
{
  if (argc!=2)
  {
    cout << "Using: tcpselect port\n";
    cout << "Example: ./tcpselect 5005\n\n";
    return -1;
  }

  int listenfd;  // 监听socket

  // 初始化监听端口
  if ( initServer(listenfd, atoi(argv[1])) == false)
  {
    perror("initServer()");
    return -1;
  }

  ////////////////////////////////////////////////////////////////////////
  //     void FD_CLR(int fd, fd_set *set);    //从位图中删除socket
  //     int  FD_ISSET(int fd, fd_set *set);  //判断
  //     void FD_SET(int fd, fd_set *set);    //socket加入到位图中
  //     void FD_ZERO(fd_set *set);           //位图初始化
  ////////////////////////////////////////////////////////////////////////
  fd_set readfds;              // 集合定义
  FD_ZERO(&readfds);           // 清空位图
  FD_SET(listenfd, &readfds);  // 加入到位图

  int maxfd = listenfd;        // 初始化取最大的临时socket
  cout << "服务端（listen_socket:" << listenfd << "）就绪。\n";

  // 外层循环，一直select，监视socket
  while(true)
  {
    // select()：等待事件发生
    struct timeval timeout;
    timeout.tv_sec = 10;    // 时间的秒
    timeout.tv_usec = 0;    // 时间的微妙

    // 读：已连接队列中有已经准备好的socket（有新的客户端连上来）
    //     接收缓存中有数据可以读（对端发送的报文已到达）
    //     tcptcp连接已断开（对端调用close()函数关闭了连接）

    // select后有事件发生会对bitmap更改，所以拷贝一份保证readfs的准确性
    fd_set tmpfds = readfds;

    /**
     * 1.bitmap中socket中最大值+1
     * 2.读事件的bitmap
     * 3.写事件的bitmap
     * 5.超时时间的数据结构
     */

    /* 
     * 阻塞监听事件，有事件相应select()函数就返回，置有事件发生的socket为1
     * ifds：事件变化的个数或者监视结果
     * 超时时间暂时不设置
    */
    int ifds = select(maxfd+1, &tmpfds, NULL, NULL, NULL);

    if (ifds < 0)    // 函数调用失败
    {
      perror("select() failed");
      break;
    }

    if (ifds == 0)   // 超时
    {
      perror("select() timeout");
      continue;
    }

    // 有事件变化
    // 遍历所有在位图中的socket？注意：是从0遍历到最大的那个socket，所以要用<=
    for (int eventfd=0; eventfd<=maxfd; eventfd++)
    {
      // 只需要定位有事件发生的socket
      if (FD_ISSET(eventfd, &tmpfds) == 0)  continue;  

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
        FD_SET(clientfd, &readfds);
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
          FD_CLR(eventfd, &readfds);
          close(eventfd);
          for (int ii=maxfd; ii>0; ii--)   // 重新找最大的fd
          {
            // 找到一个最大的
            if (FD_ISSET(ii, &readfds) != 0)  
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
            FD_CLR(eventfd, &readfds);
            close(eventfd);
            // 找到一个最大的
            for (int ii=maxfd; ii>0; ii--)   // 重新找最大的fd
            {
              // 找到一个最大的
              if (FD_ISSET(ii, &readfds) != 0)  
              {
                maxfd = ii;
                break;
              }
            }
          }
          cout << "发送：" << buffer << endl; 
        }
      }   // 监听or通信
    }     // 遍历bitmap
  }       // 循环select

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

