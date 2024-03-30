/*
 * 程序名：dbblobtofile.cpp 本程序用于将数据库blob字段的二进制文件写入到文件中
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

  // 准备sql语句，数据库读比写更快
  stmt.prepare("select id,pic from girls where id=:1 or id=:2");

  // 绑定输入
  int iminid;
  int imaxid;
  stmt.bindin(1, &iminid);
  stmt.bindin(2, &imaxid);
  // select结果集输出
  stmt.bindout(1, &stgirl.id);
  stmt.bindoutblob(2, stgirl.pic, 200000, &stgirl.picsize);

  iminid = 1000;
  imaxid = 1001;

  // execute一定要判断返回结果，只需要select一次
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n");
    printf("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);  // sql语句+代码+结果
    return -1;
  }

  while (true)
  {
    memset(&stgirl, 0, sizeof(struct st_girl));

    if (stmt.next() != 0) break;

    // 输出文件
    char filename[1024];
    sprintf(filename, "%ld_out.jpg", stgirl.id);
    // 程序输出到文件
    buftofile(filename, stgirl.pic, stgirl.picsize);
  }

  printf("本次查询到的元组数为：%ld\n", stmt.m_cda.rpc);

  return 0;
}
