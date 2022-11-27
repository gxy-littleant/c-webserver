#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <pthread.h>
#include <list>
#include <exception>
#include <cstdio> //输出输出
#include "locker.h"





// 线程池类，定义成模板是为了代码的复用,模板参数T是任务类

template<typename T>
class threadpool{
public:
    threadpool(int thread_num = 8,int max_requests =10000);
    ~threadpool();
    bool append(T* request); 

private:
    static void * worker(void * arg);
    void run();

private:
    // 线程的数量
    int m_thread_num;

    // 线程池数组，用于装线程
    pthread_t * m_threads;

    //请求队列中最多允许的，等待处理的请求数量
    int m_max_requests;

    //请求队列
    list < T*> m_workqueue;

    //互斥锁
    locker m_queuelocker;

    //信号量用来判断是否有任务需要处理
    sem m_queuestat;

    // 是否结束线程
    bool m_stop;
};

template<typename T>
threadpool<T> ::threadpool(int thread_num,int max_requests):
    m_thread_num(thread_num),m_max_requests(max_requests),
    m_stop(false),m_threads(NULL){
        
    if((thread_num <=0) || max_requests <=0)
        throw exception();  
    
    m_threads = new pthread_t[m_thread_num];
    if(!m_threads){
        throw exception();
    }

    // 创建thread_num个线程，并将它们设置为线程脱离
    for(int i=0;i<thread_num;i++){
        printf("create the %dth thread\n",i);

        if(pthread_create(m_threads + i,NULL,worker,this)!=0){
            delete [] m_threads;
            throw exception();
        }
        // 出错，则释放，并抛出异常
        if(pthread_detach(m_threads[i])){
            delete [] m_threads;
            throw exception();
        }
    }
}
    
template<typename T>
threadpool<T>::~threadpool(){
    delete [] m_threads;
    m_stop = true;
}

template<typename T>
bool threadpool<T>::append(T * request){
    m_queuelocker.lock();
    if(m_workqueue.size() > m_max_requests ){
        m_queuelocker.unlock(); 
        return false;
    }

    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
void * threadpool<T>::worker(void* arg){
    threadpool * pool = (threadpool *) arg;
    pool->run();
    return pool;
}

template<typename T>
void threadpool<T>::run(){
    while(!m_stop){
        m_queuestat.wait();//若有信号量，则信号量的值-1，没有信号量，则阻塞
        m_queuelocker.lock();
        if(m_workqueue.empty()){
            m_queuelocker.unlock();
            continue;
        }

        T * request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();

        if(!request){
            continue;
        }
        request->process();
        
    }
}

#endif