/**
 * book02.cpp：本程序用于socket客户端的实现
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
  if (argc!=3)
  {
    printf("Using: book02 ip port\n");
    printf("Example: ./book02 192.168.0.74 5005\n\n");
    return -1;
  }

  // 创建socket
  int socketfd;
  if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    return -1;
  }

  // 配置服务器信息，连接
  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  int addrlen = sizeof(serveraddr);
  serveraddr.sin_family = AF_INET;  // 协议簇
  serveraddr.sin_port = htons(atoi(argv[2]));  // 端口
  struct hostent* h = NULL;
  if ((h=gethostbyname(argv[1])) == NULL)
  {
    printf("gethostbyname(%s) failed.\n", argv[1]);
    return -1;
  }
  memcpy(&serveraddr.sin_addr, h->h_addr, h->h_length);  // IP地址

  // 主动连接
  if (connect(socketfd, (struct sockaddr*)&serveraddr, (socklen_t)addrlen) != 0)
  {
    perror("connect");
    close(socketfd);
    return -1;
  }

  // 通信
  char buffer[1024];
  for (int ii=0;ii<100000000;ii++)
  {
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "这是第%d个超女，编号：%010d。", ii+1, ii);
    int iret;
    if ((iret=send(socketfd, buffer, 1024, 0)) <= 0)
    {
      perror("send");
      break;
    }
    printf("iret=%d，发送：%s\n", iret, buffer);

sleep(1);

    if ((iret = recv(socketfd, buffer, sizeof(buffer), 0)) <= 0)
    {
      printf("iret=%d\n", iret);
      break;
    }
    printf("iret=%d, 接收：%s\n", iret, buffer);

  }

  close(socketfd);
  return 0;
}


