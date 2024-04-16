# qxidc | 气象数据中心项目

## 环境

* 主机：centos7，gcc、g++内置，gdb：7.8，cmake：3.14

## 配置Cmake编译

基本忘光了，课程也已经更新了，还是学回旧的课程吧

* g++ -L -l 编译选项是针对静态库来编译的
* 使用cmake编译来跟着视频走

## 使用框架的日志类打印日志

## 将文件中的数据读取到程序内的容器

* 循环读取文件一行
* 分割内容

## 生成模拟观测数据

## 将观测数据写入文件

* 创建临时文件+写入临时文件+重命名，这样能避免写入文件时其他程序读到不完整的数据
* stat 显示文件的状态，同时加上可以选择数据文件时间的选项

## 支持生成数据格式为xml

## 支持生成数据格式为json

#### 多进程避免产生僵尸进程的方法

* 处理子进程退出SIGCHLD的信号
* 忽略子进程SIGCHLD信号
* 父进程阻塞等待子进程退出发出SIGCHLD信号，一旦有一个子进程推出，父进程wait函数就返回

## 创建服务调度程序

* 多进程，父进程先退出，子进程成为孤儿进程，由系统1号进程托管
* 多进程，子进程先退出，父进程如果没有处理对子进程发出的SIGCHLD信号，则子进程成为僵尸进程占用系统进程资源

## 创建并写入一块共享内存

* ipcs -m 查看共享内存
* ipcrm -m shmid 删除共享内存

## 给共享内存加入简单的二元信号量锁

* 信号量值包括二元锁（互斥锁），用0和1表示
* 值如果表示可用资源数，可以实现生成/消费者模型，忘了。。。
* p操作或者wait操作，代表给资源上锁或用去一个资源
* v操作或者post操作，用于释放一个资源

## 测试守护进程

* 守护进程以及添加心跳信息的服务程序都要访问`共享内存`中的进程信息
* 而同时竞争访问修改同一块内存需要互斥，这时就要用上信号量了

## 定时压缩超时的数据文件

## 定时删除超时的数据文件

## 用shell脚本启动服务程序

## 配置mysql到c程序

* 配置好相关引用(库)目录，以及额外需要的cpp源码和库名，就可以编译编译了
* 还需要配置好服务器的LD_LIBRARY_PATH选项

## 使用sql语句创建一个表

## 使用sql语句插入一些数据

## 使用sql语句更新表数据

* prepare，然后绑定序号

## 使用sql语句查询表数据

* prepare->bindin/bindout->execute->next循环
* MySQL对于日期型的值，str_to_date()函数可以对其输入字符串
* date_format()可以输出字符串

## 使用sql语句删除表数据

## 输入blob二进制字段到数据库

* 注意好数据库、程序、外部文件的理解
* 框架已经封装好了，只需要把blob当作值一样写入sql语句即可

## 输出blob二进制字段到数据库

## 怎么插入空的字段

* 不指明字段插入，修改prepare sql语句不合适
* 用空字符串来插入，框架内置判别是否是MySQL的空类型
* params_in_is_null=true
* MySQL用空字符串''插入decimal类型会报错的

## 把站点数据放到数据库中

* 回顾守护进程：程序设定自己的超时时间放在共享内存
* 守护进程定时执行检测共享内存里守护进程是否超时
* 如超时，则kill掉
* 1；这个添加进数据库的操作有一个记录时间字段，记录用户插入或者更新站点数据的元组
* 2；引用了Mysql的库文件

## 把站点观测数据XML放到数据库中

* primary key (obtid, ddatetime)和IDX_ZHOBTMIND_1 (ddatetime, obtid)的区别
* where obtid=:1 and ddatetime=:2 查询时用主键
* where ddatetime=:1 and obtid=:2 查询时用索引
* statement的初始化需要connection对象，这点需要注意
* 理解好观测数据和数据库数据格式的转换

## 尝试把字符串分割和sql语句的执行封装成对象

* 注意好要封装的内容就行
* 对于日志类、数据库连接的时机这些不是封装的内容需要注意代码的书写

## 支持csv格式的文件数据入库

* 修正一些错误封装细节，比如结构体的初始化，conn对象的加载
* 同时检测csv和xml文件来支持对数据库的插入

## 定期执行sql脚本

* 内置的命令行：mysql -uroot -ppwd -Ddb_qxidc < xxx.sql
* 执行shell工具不支持输入重定向
* 所以编写一个读取sql文件然后执行这些个语句

## cmake编译ftp静态库

* 因为ftp库在clion没有可执行的入口，所以编译只能在Linux中手打
* 在任意一个 CMakeLists.txt的目录下：
* cmake --build {outputpath} [--target targetname]
* 静态库的输出目录配置在具体的CMakeLists.txt中

## ftpgetfiles:将XML参数解析到程序中

* ftp下载的检验是通过文件的时间是否相一致来判断文件正确性，同样的aaa和bbb它们的大小是一样的，无法比较
* 而服务器上传的过程保证了文件不会发生变化，那么只需要比较文件的大小就可以

## 拉取服务器的文件

## 完善ptype传输

* ptype=2，下载完文件后删除ftp服务器的文件
* ptype=3，下载完备份文件到新的目录

## 新开一个多线程

* ps -ef查看进程编号
* ps -Lf + 进程编号，查看进程的线程信息
* LWP线程id，NLWP线程数目
* 线程拥有独立的TCB

## 线程的独立性

* 线程如果调用了exit()，出现core dump现象，或者给进程发信号
* 整个进程都会退出

## 线程退出

* pthread_exit(); pthread_calcel()，线程return

## 线程函数的参数传递

* 每个线程创建到执行的时间间隔是不一样的，有的进程快，有的进程慢
* 可以把8字节的void*当作值传给线程函数
* 也可以把具体的动态变量（int，结构体）的地址传进去，然后在线程函数里用完释放这个地址

## 接受线程函数的返回值

* pthread_join()第二个的参数就是一个8字节的void**二级指针，应把他当作8字节的空间来使用比较方便
* 只能封装当一级指针然后再取地址这样操作二级指针
* 也可以将结构体地址传来传去

## 子线程分离主线程，线程退出时资源由系统回收

* 要么等到join之后由主线程释放资源
* 要么自己分离主进程，自己释放资源

## 构建网络服务程序
* htons(5005)：本地intel大字节端5005 = 0001 0011 1000 1101 ==> 36115 = 1000 1101 0001 0011
* 大字节端（intel）：只对于大于1字节的情况下，高位的字节放在内存的低位（0x00，栈底），低位的字节放在内存的高位（内存想象成一个栈）
* 小字节端（网络服务）：相反

## 封装网络服务程序
* string容器：
  * data()：返回容器字符的char*地址
  * resize()：动态改变容器大小
  * [index]和at(index)，返回index的char元素，[]运算没有范围检查

## 构建多进程网络服务端
* 子进程fork后一定要在业务完成的位置退出程序
* ls /proc/{pid}/fd 查看进程打开的socket，要处理好父进程和子进程多余的socket

# 尝试构建多线程的网络服务端
* 因为TcpServer对象的连接socket只有一个，每次连上了都会有新的socket产生，所以要在子线程中保留新连上来的socket。
* 因为框架的TcpRead/Write解决了分包，粘报问题，报文的长度载入了报文的前部，不一样的报文内容，需要改客户端。
* 算了，还是没用

## 增加线程退出处理
* 线程的创建和销毁需要一点时间
* 所以创建的时候需要保留客户端连接的socket，否则有新的客户端连上来时clientfd会改变
* 线程销毁时，如果直接执行完pthread_cancel后exit退出进程，那么很有可能线程的清理函数thcleanup还没有执行完
* 线程的清理函数有如下触发方式:
  * pop出栈时
  * pthread_cancel调用时
  * 线程退出时

## 主机主动退出后不需要等到2MSL的挥手时间直接重新打开服务端口

## 给特定的共享空间或者变量加自选锁
* 自旋锁：针对的是等待时间不长，类似于互斥锁的变量
* 除此之外，如果服务端需要打印日志，还需要向日志对象加锁，日志对象的时间函数那些也需要用线程安全的
  
## 构建基本的select网络通信服务
* 读事件
  * 已连接队列中有已经准备好的socket（有新的客户端连上来）
  * 接收缓存中有数据可以读（对端发送的报文已到达）
  * tcptcp连接已断开（对端调用close()函数关闭了连接）
* 写事件
  * 发送缓冲区没有满，可以写入数据（可以向对端发送报文）
* select(readfds)：根据已有的位图bitmap信息，在已存在(1)的位图信息里，保留发生事件的位置，清空没有发生事件的位置。个人猜测，所以要留多一份bitmap
* 需要维护：
  * 一个最大maxfd，因为每次都要遍历位图。
  * socket资源。
  * 位图信息。

## select的更多知识
* 每次有select到事件，程序需要拷贝一份bitmap（128字节），还需要从程序（用户态）拷贝一份到内核（内核态），在
  更多的连接上来时开销更大
* 每次select上来不处理事件，下次还是通知处理事件，所以事件不会丢失
* select最多支持1024的事件，设置更多的支持量效率也会低

## 构建基本的poll网络通信服务
* poll需要维护
  * 一个最大maxfd，因为每次都要定位监听事件的socket
  * poll结构体里的fd和event
* 以poll结构体数组来监视socket的变化
  * fd：需要监视的socket编号
  * event：监视fd的类型，一般是读事件(POLLIN:监听事件，通信缓冲区可读，连接断开)
  * revent：监视fd中POLL所返回的事件（有事件出发时，返回事件对应的宏）
* &按位与：二目运算符，每个二进制位只有都是1才为1
* poll的超时机制用于监听tcp socket接收报文的超时开发
  
## 构建基本的epoll网络通信服务
* 句柄（Handle）是一个用于标识和访问对象或项目的标识符，epoll句柄占用一个fd
* 因为poll的访问既是下标又是socket，而epoll中只有发生事件的epoll结构体的fd，所以循环判断事件的类型只是结构体里边的fd数据
 
 ## 测试客户端的非阻塞
 * 非阻塞：个人理解，如果调用一些阻塞函数前accpet(),connect(),send(),recv()前没有处理对象，那么这些阻塞函数会返回错误
 * 设置非阻塞的socket在connect()都会报EINPROGRESS错误，代表无法立即处理，需要忽略这个错误
 * 判断客户端连接成功与否可以用poll来判断socket的可写的超时状态
 * 像服务端设置非阻塞的状态，当listenfd设置后，如果队列没有可连接socket时，accpet会报EAGAIN，send(),recv()同样

 ## 暂存：tcpepoll的非阻塞边缘触发

 ## 学习一些makefile和c11知识
 * 写一个符合格式的makfefile，和认识更多的makefile写法
 * c11的智能指针、类继承的一些关键字

 ## tcpepoll的非阻塞边缘触发
 * 完成：
   * epoll的listenfd的非阻塞测试，sh tmp2.sh，300个socket连接请求都能完成处理
   * epoll的recv的非阻塞测试，每一次只接收5字节报文，非阻塞测试能否取完所有的字节
 * 非阻塞的socket，需要写循环判断errno，只要返回-1，再判断errno值，作相对应得处理