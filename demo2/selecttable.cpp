/*
 * 程序名：createtable.cpp 本程序用于
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

  // 程序变量用于接收结果集
  struct st_girl
  {
    long id;         // 超女编号
    char name[30];   // 姓名
    double weight;   // 体重
    char btime[20];  // 出生时间
  } stgirl;

  // 绑定连接
  sqlstatement stmt(&conn);

  // 准备sql语句
  stmt.prepare("select id,name,weight,date_format(btime,'%%Y-%%m-%%d %%H:%%i:%%s') \
               from girls where id between :1 and :2");
  // 绑定输入
  int iminval;
  int imaxval;
  stmt.bindin(1, &iminval);
  stmt.bindin(2, &imaxval);
  // 绑定输出，分别对应输出的属性顺序
  stmt.bindout(1, &stgirl.id);
  stmt.bindout(2, stgirl.name, 29);
  stmt.bindout(3, &stgirl.weight);
  stmt.bindout(4, stgirl.btime, 19);

  iminval = 1002;
  imaxval = 1004;
  // execute一定要判断返回结果，只需要select一次
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n");
    printf("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);  // sql语句+代码+结果
    return -1;
  }

  // next取结果集
  while (true)
  {
    memset(&stgirl, 0, sizeof(struct st_girl));  // 清空结果接收器

    // 1403结果集为空，其他非0是错误，有可能查询的过程中（数据量大）和数据库断连
    if (stmt.next() != 0) break;
    printf("id:%ld，姓名：%s，体重：%.2lf，出生时间：%s\n", \
      stgirl.id, stgirl.name, stgirl.weight, stgirl.btime);
  }

  printf("查询完成！本次查询到的元组数为：%ld\n", stmt.m_cda.rpc);

  return 0;
}
