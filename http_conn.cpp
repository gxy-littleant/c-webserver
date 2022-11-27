#include "http_conn.h"


int http_conn::m_epollfd = -1;//所有socket上的事件都被注册到同一个epoll对象中
int http_conn::m_users_count = 0;//统计用户数量



//设置文件描述符非阻塞
void setnonblocking(int fd){
    //获取原来的属性
    int old_flag = fcntl(fd,F_GETFL);
    //添加非阻塞属性
    int new_flag = old_flag | O_NONBLOCK;
    // 设置
    fcntl(fd,F_SETFL,new_flag);
}



//将监听的文件描述符添加到epoll中

void addfd(int epollfd,int fd,bool one_shot){

    epoll_event event;
    event.data.fd = fd;
    // 若对方连接异常断开，会触发EPOLLIN | EPOLLRDHUP ,
    // 此时可以获取到异常，不用移交给上层read/recv返回值判断是否异常
    event.events = EPOLLIN | EPOLLRDHUP;

    if(one_shot){
        event.events | EPOLLONESHOT;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event); 

    //设置文件描述符非阻塞
    setnonblocking(fd);



}

//从epoll中删除文件描述符
void removefd(int epollfd,int fd){
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0); 
    close(fd);
}


//修改文件描述符,重置socket上的EPOLLONESHOT事件
// 确保下次可读时，EPOLLIN事件能被触发
void modifyfd(int epollfd,int fd,int ev){
    epoll_event event;
    event.data.fd =fd;
    event.events = ev|EPOLLONESHOT|EPOLLRDHUP;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
}

//初始化连接
void http_conn::init(int sockfd,const sockaddr_in &addr){
    m_sockfd = sockfd;
    m_address = addr;

    //端口复用
    int reuse = 1;
    setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));

    //添加到epoll对象中
    addfd(m_epollfd,m_sockfd,true);
    m_users_count++;//总用户是+1
}

//关闭连接
void http_conn::close_conn(){
    if(m_sockfd !=-1){
        removefd(m_epollfd,m_sockfd);
        m_sockfd = -1;
        m_users_count--;//关闭一个连接，客户总数量-1
    }
}

bool http_conn::read(){
    printf("一次性读出所有数据\n");
    return true;
}


bool http_conn::write(){
    printf("一次性写完所有数据\n");
    return true;
}

//由线程池中工作线程调用的，处理http请求的入口函数

void http_conn::process(){


    //解析http请求

    printf("parse request,create response\n");

    //生成响应



}