#ifndef HTTPCONNNECTION_H
#define HTTPCONNNECTION_H

#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/mman.h> //内存映射
#include <errno.h>
#include "locker.h"
#include <sys/uio.h>

class http_conn{

public:

    static int m_epollfd;//所有socket上的事件都被注册到同一个epoll对象中
    static int m_users_count;//统计用户数量

    http_conn(){}

    ~http_conn(){}

    void process(); //处理客户端的请求  解析请求报文 找到需要的资源 拼接响应的信息 交给主线程回写响应
    void init(int sockfd,const sockaddr_in &addr);//初始化新接收的连接
    void close_conn();//关闭连接
    bool read();//非阻塞读
    bool write();//非阻塞写


private:

    int m_sockfd; //该http连接的socket
    sockaddr_in m_address; //通信的socket地址


};

#endif