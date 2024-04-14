
#include <iostream>
using namespace std;
/********
 * virutal：定义虚函数，基类指针只能调用基类对象，想要调用派生类函数，需要定义virtual关键字 
 * 但是派生类可以指针可以调用基类函数（因为是从基类继承而来的）
 * 
 */
class Base
{
public:
  virtual void func() final
  {
    cout << "Base里的final修饰的虚函数不能被重写。\n";
  }

  virtual void func1()
  {
    cout << "Base里的虚函数。\n";
  }
};

// final类不能被继承
class Base1 final
{

};

class Derrive:public Base
{
public:
  // void func(){}          // 不能重写

  void func1()      // 派生类显式声明重写基类虚函数
  {
    cout << "派生类override显式声明重写基类虚函数\n";
  }

};

// class Derrive1:public Base1
// {

// };

int main(int argc, char* argv[])
{
  Derrive dd;
  dd.func();
  dd.func1();

  Base& bb = dd;
  bb.func1();
  
  
  return 0;
}