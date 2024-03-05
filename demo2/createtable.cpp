/*
 * 程序名：createtable.cpp 本程序用于连接MySQL创建一个表
 * 时间：2024-03-05 15:20
*/

#include "_mysql.h"  // 开发框架所需要的头文件

int main(int argc, char *argv[])
{
  connection conn;  // 数据库连接类
  // ip,数据库用户，密码，数据库，端口
  if (conn.connecttodb("192.168.0.74,qxidc,qxidcpwd,db_qxidc,3306", "utf8") != 0)
  {
    printf("连接数据库错误.\n%s\n", conn.m_cda.message);
    return -1;
  }

  // 绑定连接
  sqlstatement stmt(&conn);

  stmt.prepare("drop table if exists girls");
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n");
    printf("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);  // sql返回代码+结果
  }

  // 准备sql语句
  stmt.prepare("create table girls("
               "id bigint(10),"
               "name varchar(30) not null,"
               "weight decimal(8,2),"
               "btime datetime,"
               "memo longtext,"
               "pic longblob,"
               "primary key(id))");

  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n");
    printf("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);  // sql返回代码+结果
  }

  return 0;
}
