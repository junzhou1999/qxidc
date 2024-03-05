/*
 * 程序名：deletetable.cpp 本程序用于
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

  // 准备sql语句
  stmt.prepare("delete from girls where id>=:1 and id<=:2");

  // 绑定输入
  int iminid;
  int imaxid;
  stmt.bindin(1, &iminid);
  stmt.bindin(2, &imaxid);

  iminid = 1002;
  imaxid = 1003;

  // execute一定要判断返回结果
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n");
    printf("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);  // sql语句+结果代码+结果
    return -1;
  }

  conn.commit();  // 删除需要提交事务

  printf("删除完成！本次删除了的元组数为：%ld\n", stmt.m_cda.rpc);

  return 0;
}
