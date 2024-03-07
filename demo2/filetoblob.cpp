/*
 * 程序名：filetoblob.cpp 本程序用于将图片文件存储到数据库blob字段中
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
    long id;
    char pic[200000];            // 图片缓存，大概不超200KB
    unsigned long picsize;       // 图片大小，用匹配框架的unsigned long类型
  } stgirl;

  // 绑定连接
  sqlstatement stmt(&conn);

  // 准备sql语句
  stmt.prepare("update girls set pic=:1 where id=:2");

  // 绑定输入
  stmt.bindinlob(1, stgirl.pic, &stgirl.picsize);
  stmt.bindin(2, &stgirl.id);

  for (int ii = 0; ii <= 1; ii++)
  {
    memset(&stgirl, 0, sizeof(struct st_girl));
    stgirl.id = ii + 1000;

    if (ii == 0) stgirl.picsize = filetobuf("1.jpg", stgirl.pic);
    if (ii == 1) stgirl.picsize = filetobuf("2.jpg", stgirl.pic);

    // execute一定要判断返回结果，只需要select一次
    if (stmt.execute() != 0)
    {
      printf("stmt.execute() failed.\n");
      printf("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);  // sql语句+代码+结果
      return -1;
    }
    printf("blob字段放入数据库完成！本次修改的元组数为：%ld\n", stmt.m_cda.rpc);
  }

  // 提交事务
  conn.commit();

  return 0;
}
