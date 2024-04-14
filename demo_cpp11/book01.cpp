/**
 * 程序名：book01.cpp   日期：2024年04月14日
 * 本程序用于实现auto，decltype类型推导
 */
#include <iostream>
#include <list>       // 链表

using namespace std;

int main(int argc, char* argv[])
{
  auto x = 42;    // auto根据右值自动推导类型
  decltype(x) y;  // decltype推导数据类型给变量: decltype(变量1) 变量2 ==> 定义变量2的数据类型是变量1

  y = 32;

  cout << "sizeof(y):" << sizeof(decltype(x)) << endl;
  cout << y << endl;

  list<int> lGirlNo = {12, 34, 66, 7};

  // auto自动推导类型
  cout << "------------" << endl;
  for (auto it: lGirlNo)
  {
    cout << it << endl;
  }
  // 手写一个
  for (list<int>::iterator it = lGirlNo.begin(); it!=lGirlNo.end(); it++)
  {
    cout << *it << endl;
  }


  return 0;
}