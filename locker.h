#ifndef LOCKER_H
#define LOCKER_H

#include <pthread.h> // 互斥锁
#include <exception>
#include <semaphore.h>


using namespace std;

//线程同步机制封装类

//互斥锁类
class locker{
public:
    //构造函数
    locker(){
        // 第二个参数是属性
        if(pthread_mutex_init(&m_mutex, NULL)!=0){
            throw exception();
        }
    }
    //析构函数
    ~locker(){
        pthread_mutex_destroy(&m_mutex);
    }

    bool lock(){
        return pthread_mutex_lock(&m_mutex)==0;
    }

    bool unlock(){
        return pthread_mutex_unlock(&m_mutex)==0;
    }

    pthread_mutex_t * get(){
        return &m_mutex;
    }



private:    
    pthread_mutex_t m_mutex;
};


// 条件变量:可以引起线程阻塞和解除线程阻塞
// 配合互斥锁实现线程同步
class cond{

public:
    cond(){
        if(pthread_cond_init(&m_cond,NULL)!=0){
            throw exception();
        }
    }
    ~cond(){
        pthread_cond_destroy(&m_cond);
    }

    bool wait(pthread_mutex_t * mutex){
        return pthread_cond_wait(&m_cond,mutex)==0;
    }

    bool timewait(pthread_mutex_t * mutex,struct timespec t){
        return pthread_cond_timedwait(&m_cond,mutex,&t)==0;
    }
    //唤醒一个或多个线程
    bool signal(pthread_mutex_t * mutex){
        return pthread_cond_signal(&m_cond) ==0;
    }
    //唤醒所有线程
    bool broadcast(){
        return pthread_cond_broadcast(&m_cond)==0;
    }

private:
    pthread_cond_t m_cond;
};

// 信号量
class sem{

public:
    sem(){
        if(sem_init(&m_sem,0,0)!=0){
            throw exception();
        }
    }

    sem(int num){
        if(sem_init(&m_sem,0,num)!=0){
            throw exception();
        }
    }

    ~sem(){
        sem_destroy(&m_sem);
    }
    //等待信号量
    bool wait(){
        return sem_wait(&m_sem) ==0;
    }
    //增加信号量
    bool post(){
        return sem_post(&m_sem) ==0;
    }
    
private:
    sem_t m_sem;  

};

#endif