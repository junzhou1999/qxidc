/**
 * tcpepoll2.cpp：本程序用于socket服务端的实现，支持epoll特性的IO复用技术，测试epoll的边缘触发
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
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

// 初始化listen socket，传参
bool initServer(int& listenfd, const unsigned short in_port);

// 监听端口接受客户端的连接
bool Accept(const int& listenfd, int& clientfd, struct sockaddr_in& clientaddr);

// 设置socket的标志属于非阻塞
int setNoBlock(int fd);

int main(int argc, char* argv[])
{
  if (argc!=2)
  {
    cout << "Using: tcpepoll port\n";
    cout << "Example: ./tcpepoll2 5005\n";
    cout << "         ./tcpclient2 127.0.0.1 5005\n\n";

    return -1;
  }

  int listenfd;  // 监听socket

  // 初始化监听端口
  if ( initServer(listenfd, atoi(argv[1])) == false)
  {
    perror("initServer()");
    return -1;
  }

  // 设置监听socket非阻塞，accept会返回-1
  setNoBlock(listenfd);

  // 创建epoll句柄
  int epollfd = epoll_create(1); 

  // 用于处理单个事件（包含socket信息）的结构体
  struct epoll_event ev;                // 这里是listenfd监听事件的结构体
  ev.data.fd = listenfd;                // 指定事件的自定义数据，会随epoll_wait()一并返回
  // ev.events = EPOLLIN;                  // 打算让epoll监听socket的读事件
  ev.events = EPOLLIN|EPOLLET;          // 监听，启用边缘触发的方式
  
  epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);   // 把需要监听的socket加入到epollfd句柄中

  // 初始化
  cout << "服务端（listen_socket:" << listenfd << "）就绪。\n";

  const int MAX_EVENTS = 10;
  struct epoll_event evs[MAX_EVENTS];   // 声明epoll返回的事件数据结构体
static int count;
  // 事件循环
  while(true)
  {
    /* 
     * 阻塞监听事件，有事件响应epoll()函数就返回
     * ifds：事件变化的个数或者监视结果
     * 1.epoll句柄
     * 2.监听事件的结构体数组地址
     * 3.监听事件数目
     * 4.超时时间（毫秒ms）：-1，永远等待
    */
    int infds = epoll_wait(epollfd, evs, MAX_EVENTS, -1);

    if (infds < 0)    // 函数调用失败
    {
      perror("epoll() failed");
      break;
    }

    if (infds == 0)   // 超时
    {
      perror("epoll() timeout");
      continue;
    }

    // 遍历发生的事件，infds返回发生的事件数
    for (int ii=0; ii<infds; ii++)
    {
      // 属于监听事件的
      if (evs[ii].data.fd == listenfd)
      {
        while (true)
        {
          // 有客户端连上来
          int clientfd;  struct sockaddr_in clientaddr;
          if (Accept(listenfd, clientfd, clientaddr) == true)   break;

          //测试并发的多个连接数
          // cout << "第" << ++count << "个客户端连上了。\n";
          cout << "客户端（socket:" << clientfd <<  "）已连接。" << endl;

          // socket加入的操作，这里是clientfd加入监视
          setNoBlock(clientfd);
          ev.data.fd = clientfd;
          ev.events = EPOLLIN|EPOLLET;
          epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
        }
      }
      else 
      // 属于通信事件
      {
        string buffer;  buffer.clear();  buffer.resize(1024);
        while (true)
        {
          // 接收报文失败情况
          if (recv(evs[ii].data.fd, &buffer[0], 5, 0) <= 0)
          {
            // 通信断开的一些操作
            cout << "客户端（socket:" << evs[ii].data.fd << "）已断开。\n"; 

            close(evs[ii].data.fd);
            // 从epoll中删除客户端的socket，如果socket被关闭了，会自动从epollfd句柄中删除，多以以下代码可不启用
            // epoll_ctl(epollfd, EPOLL_CTL_DEL, evs[ii].data.fd, NULL);
          }
        }
        else
        // 通信事件，接收报文成功
        {
          cout << "接收：" << buffer << endl;

          buffer = "ok";
          if (send(evs[ii].data.fd, buffer.data(), strlen(buffer.data()), 0) <= 0)
          {
            cout << "客户端（socket:" << evs[ii].data.fd << "）已断开。\n"; 

            close(evs[ii].data.fd);

          }
          cout << "发送：" << buffer << endl; 
        }
      }   // 监听or通信
    }     // 遍历epoll结构体数组
  }       // 循环epoll

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
 * 阻塞的监听端口accept()只能是EAGAIN才能跳出循环，代表队列中没有socket了
*/


/**
 * @brief 
 * 
 * @param listenfd 
 * @param clientfd 
 * @param clientaddr 
 * @return true 退出循环
 * @return false 继续循环，边缘触发还有监听的客户端没accept()处理
 */
bool Accept(const int& listenfd, int& clientfd, struct sockaddr_in& clientaddr)
{
  int addrlen = sizeof(clientaddr);
  if ((clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&addrlen)) < 0 && errno == EAGAIN)
  {
    // cout << "clientfd:" << clientfd << ",errno:" << errno << ",EAGAIN:" << EAGAIN <<endl;
    return true;
  }

  return false;
}

// 设置socket的标志属于非阻塞
int setNoBlock(int fd)
{
  int flags;

  if ( (flags=fcntl(fd, F_GETFL, 0)) == -1)  flags = 0;

  return fcntl(fd, F_SETFL, flags|O_NONBLOCK);
}
