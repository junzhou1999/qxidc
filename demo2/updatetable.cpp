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

  // 准备数据插入数据库
  struct st_girl
  {
    long id;          // 超女编号
    char name[31];    // 姓名
    double weight;    // 体重
    char btime[20];   // 出生时间
  } stgirl;

  // 绑定连接
  sqlstatement stmt(&conn);

  // 准备sql语句
  stmt.prepare("update girls set name=:1,weight=:2,btime=str_to_date(:3,'%%Y-%%m-%%d %%H:%%i:%%s') where id=:4");
  // 绑定数据
  stmt.bindin(1, stgirl.name, 31);  // len应是表字段+1
  stmt.bindin(2, &stgirl.weight);
  stmt.bindin(3, stgirl.btime, 20);
  stmt.bindin(4, &stgirl.id);

  for (int ii = 1; ii < 5; ii++)
  {
    memset(&stgirl, 0, sizeof(struct st_girl));

    stgirl.id = ii + 1000;  // 根据id号来修改记录
    sprintf(stgirl.name, "玉环%05d号girl", ii + 1);
    stgirl.weight = 59 + (double) ii / 10;
    sprintf(stgirl.btime, "2022-03-05 18:29:%2d", ii);

    // execute一定要判断返回结果
    if (stmt.execute() != 0)
    {
      printf("stmt.execute() failed.\n");
      printf("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);  // sql语句+代码+结果
      return -1;
    }

    printf("成功操作了%d条记录。\n", stmt.m_cda.rpc);   // rpc结果集
  }

  printf("update tables girls ok.\n");
  // 操作默认需要手动提交事务
  conn.commit();

  return 0;
}
