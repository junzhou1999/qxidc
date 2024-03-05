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
    long id;         // 超女编号
    char name[30];   // 姓名
    double weight;   // 体重
    char btime[20];  // 出生时间
  } stgirl;

  // 绑定连接
  sqlstatement stmt(&conn);

  // 先绑定数据，sql主体没有改变，只需prepare一次
  stmt.prepare("insert into girls(id,name,weight,btime) "
               "values(:1,:2,:3,str_to_date(:4, '%%Y-%%m-%%d %%H:%%i:%%s'))");  // 用:序号来绑定
  // 如果重新prepare了，那么也要重新绑定
  // 绑定只是单纯替换了(:序号)这一环节，其他的部分也可以用在函数中，或者再加上运算
  stmt.bindin(1, &stgirl.id);
  stmt.bindin(2, stgirl.name, 29);
  stmt.bindin(3, &stgirl.weight);
  stmt.bindin(4, stgirl.btime, 19);

  for (int ii = 0; ii < 5; ii++)
  {
    memset(&stgirl, 0, sizeof(struct st_girl));

    stgirl.id = ii + 1000;
    sprintf(stgirl.name, "西施%05d号girl", ii + 1);
    stgirl.weight = 52 + (double) ii / 10;
    sprintf(stgirl.btime, "2024-03-05 18:29:%2d", ii);

    if (stmt.execute() != 0)
    {
      printf("stmt.execute() failed.\n");
      printf("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);  // sql返回代码+结果
      return -1;
    }
    printf("%s\n", stmt.m_sql);  // 绑定的值不会显示出来
    printf("成功操作了%d条记录。\n", stmt.m_cda.rpc);   // rpc结果集
  }

  // 操作默认需要手动提交事务
  conn.commit();

  return 0;
}
