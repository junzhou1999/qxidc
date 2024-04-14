/**
 * 程序名：book03.cpp   日期：2024年04月14日
 * 本程序用于解释三种智能指针
 * unique_ptr：这种指针只能移动，不能复制，就是不能当作右值
 * shared_ptr：当最后一个指向资源的指针被释放时，内存才会被释放
 * weak_ptr：解决shared_ptr的循环引用，shared_ptr对象中有shared_ptr成员，把shared_ptr成员改为weak_ptr后，
 * 继续保持shared_ptr的特性
 */

#include <iostream>
#include <string>
#include <memory>   // 智能指针

class AA
{
public:
  std::string m_name;
  AA(){std::cout << "调用无参构造函数。\n";}
  AA(const std::string& name):m_name(name)
  {
    std::cout << "调用构造函数AA(" << name << ")\n";
  }
  ~AA(){std::cout<<"调用析构函数~AA()\n";}
};

void func1();
void func2();
void func3();

int main(int argc, char* argv[])
{
  func1(); 
  std::cout<<"------------------\n";
  func2();
}

void func1()
{
  // std::unique_ptr<AA> uq = new AA();        // 不能从裸指针赋值给智能指针
  // 用括号()里的值初始化智能指针，注意智能指针只可以管理new出来的内存
  std::unique_ptr<AA> uq(new AA("褒姒"));      
  
  // std::unique_ptr<AA> uq1 = uq;             // 不能复制智能指针
  std::unique_ptr<AA> uq1;
  uq.swap(uq1);                   // 交换了指针的内存控制权。

  if (uq == nullptr)  std::cout<< "uq已是空指针。\n";

  std::cout << uq1->m_name << std::endl;

  // uq1 = nullptr;
  uq1.reset();                   // 置空或者调用reset那么内存也会清空

  std::cout << "end" << std::endl;
}

void func2()
{
  std::shared_ptr<AA> sp(new AA("貂蝉")), sp1, sp2;
  
  sp1 = sp2 = sp;

  std::cout << sp2.use_count() << std::endl;

  sp.reset();   // 剩sp1, sp2

  std::cout << sp2->m_name << std::endl;

  sp1.reset();  // 剩sp2

  std::cout << sp2->m_name << std::endl;

  std::weak_ptr<AA> wk = sp1;   // 弱引用的不影响，没有->和*运算

  std::cout << wk.lock() << std::endl;         // weak提升shared指针判断shared指针是否为空，不是内存资源是否释放

  sp2.reset();  // 都没了

  // std::cout << sp2->m_name << std::endl;    // 这句会报错

  std::cout << "end" << std::endl;

}

void func3()
{

}