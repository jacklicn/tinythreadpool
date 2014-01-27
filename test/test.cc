/*
 *  Project   : TinyThreadPool
 *  File      : test.cc
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#include <iostream>
#include <sstream>
#include "ThreadPool.h"

using namespace TTP;


class MyThread : public Thread
{
public:
    MyThread() {
        setId(21);
        setName("MyThread");
    }
    ~MyThread(){}
    void run() {
        std::cout << getName() << "(" << getId() << ") run ok !" << std::endl;
    }
};

class MyTask : public Task
{
public:
    MyTask(int j){m_num = j;}
    ~MyTask(){}
    void run() {
        std::cout << "Task (" << m_num << ") run ok !" << std::endl;
    }
private:
    int m_num;
};

class MyPtask : public Task
{
public:
    MyPtask():m_num(21) {
        std::cout << "pTask (" << m_num << ") create ok !" << std::endl;
    }
    ~MyPtask() {
        std::cout << "pTask (" << m_num << ") delete ok !" << std::endl;
    }
    void run() {
        std::cout << "pTask (" << m_num << ") run ok !" << std::endl;
        delete this;
    }
private:
    int m_num;
};

void testDirectExecution()
{
    /*Declare a Thread Pool with Min 2 and Max 5 Threads*/
    ThreadPool pool(2,5);
    /*Create a Task*/
    MyTask task1(1);
    MyTask task2(2);
    MyTask task3(3);
    MyTask task4(4);
    MyTask task5(5);
    /* Start Thread Pool*/
    pool.start();
    /*Execute The Task*/
    pool.execute(task1);
    pool.execute(task2);
    pool.execute(task3);
    pool.execute(task4);
    pool.execute(task5);
    /*Wait for completion of all Tasks*/
    pool.joinAll();
}

void testPrioritizedExecution()
{
    /*Declare a Thread Pool with Min 2 and Max 5 Threads, with Low 1 and 4 High Priority*/
    ThreadPool pool(2,5,1,4);
    /*Create a Task*/
    MyTask task6(6);
    MyTask task7(7);
    MyTask task8(8);
    MyTask task9(9);
    MyTask task10(10);
    /* Start Thread Pool*/
    pool.start();
    /*Execute The Tasks on priority*/
    pool.execute(task6,2);
    pool.execute(task7,4);
    pool.execute(task8,4);
    pool.execute(task9,1);
    pool.execute(task10,4);
    /*Wait for completion of all Tasks*/
    pool.joinAll();
}

void testScheduledExecution()
{
    /*Declare a Thread Pool with Min 2 and Max 5 Threads*/
    ThreadPool pool(2,5);
    /*Create a Task*/
    MyTask task11(11);
    MyTask task12(12);
    MyTask task13(13);
    MyTask task14(14);
    MyTask task15(15);
    /* Start Thread Pool*/
    pool.start();
    /*Schedule a task to be executed after the defined delay period*/
    pool.schedule(task11,10,TimeUnit::MILLISECONDS);
    pool.schedule(task12,10,TimeUnit::SECONDS);
    pool.schedule(task13,1,TimeUnit::HOURS);
    pool.schedule(task14,10,TimeUnit::DAYS);
    pool.schedule(task15,110,TimeUnit::MILLISECONDS);
    /*Wait for completion of all Tasks*/
    pool.joinAll();
}

void testDirectScheduledExecution()
{
    /*Declare a Thread Pool with Min 2 and Max 5 Threads*/
    ThreadPool pool(2,5);
    MyTask task16(16);
    MyTask task17(17);
    MyTask task18(18);
    MyTask task19(19);
    MyTask task20(20);
    /* Start Thread Pool*/
    pool.start();
    /*Schedule a task to be executed after the defined delay period*/
    pool.schedule(task16,10,TimeUnit::MILLISECONDS);
    pool.schedule(task17,10,TimeUnit::SECONDS);
    /*Execute the task*/
    pool.execute(task18);
    pool.schedule(task19,10,TimeUnit::DAYS);
    pool.execute(task20);
    /*Wait for completion of all Tasks*/
    pool.joinAll();
}


int main()
{
    /*Test the thread use the overwrite run function*/
    MyThread mythread;
    mythread.execute();
    mythread.join();
    /*Declare a Thread Pool with Min 2 and Max 5 Threads*/
    ThreadPool pool(2,5);
    MyPtask *ptask = new MyPtask;
    /* Start Thread Pool*/
    pool.start();
    /*Execute The Task*/
    pool.execute(ptask);
    /*Wait for completion of all Tasks*/
    pool.joinAll();
    /*Test the Direct Thread Pooling mechanism*/
    testDirectExecution();
    /*Test the Scheduled Thread Pooling mechanism*/
    testScheduledExecution();
    /*Test the Priority Driven Thread Pooling mechanism*/
    testPrioritizedExecution();
    /*Test the Mixed Thread Pooling mechanism*/
    testDirectScheduledExecution();
    return 0;
}
