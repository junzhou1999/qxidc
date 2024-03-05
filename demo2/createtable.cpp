/*
 * 程序名：createtable.cpp 本程序用于
 * 时间：2024-03-05 15:20
*/

#include "_mysql.h"

int main(int argc, char *argv[])
{
  connection conn;
  if (conn.connecttodb("192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306", "utf8") != 0)
  {
    printf("连接数据库错误.\n");
    return -1;
  }
  printf("连接成功！\n");
  return 0;
}
