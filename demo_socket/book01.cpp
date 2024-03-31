/**
 * book01.cpp：本程序用于socket服务端的实现
*/
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
int main(int argc, char* argv[])
{
  if (argc!=2)
  {
    printf("Using: book01 port\n");
    printf("Example: ./book01 5005\n\n");
    return -1;
  }

  // 创建socket
  int listenfd;
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    return -1;
  }

  // 配置连接参数，绑定
  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  // serveraddr.sin_addr.s_addr = inet_addr("118.89.50.198"); // 指定ip地址。
  serveraddr.sin_port = htons(atoi(argv[1]));
  if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) !=0 )
  {
   perror("bind");
   return -1; 
  }

  // sockets设置为监听模式，参数3为可以容纳的established队列个数-1
  if (listen(listenfd, 3) != 0 )
  {
    perror("listen");
    return -1;
  }

  // listen socket阻塞等待，可以从这里分叉了
  int clientfd;
  struct sockaddr_in clientaddr;
  int addrlen = sizeof(clientaddr);
  clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&addrlen);
  printf("客户端（%s）已连接。\n", inet_ntoa(clientaddr.sin_addr));

  // 双方开始通信
  char buffer[1024];
  while(true)  // 服务端接收来自客户端的交流请求
  {
    memset(buffer, 0, sizeof(buffer));
    int iret;
    if ((iret = recv(clientfd, buffer, 1024, 0)) <= 0)
    {
      printf("iret=%d\n", iret);
      break;
    }
    printf("iret=%d，接收：%s\n", iret, buffer);

    strcpy(buffer, "ok");
    if ((iret=send(clientfd, buffer, strlen(buffer), 0)) <= 0)
    {
      perror("send");
      break;
    }
    printf("iret=%d，发送：%s\n", iret, buffer);

  }

  close(listenfd);
  close(clientfd);
  return 0;
}

