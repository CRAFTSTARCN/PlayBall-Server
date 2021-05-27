/*
    ThreadPool.hpp Created 2021/5/17
    None-template calss with template function
    By Ag2S
*/

#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <future>

class ThreadPool {
    public:
        using m_task = std::function<void()>;

    private:    

    std::mutex task_access_mutex;
    std::condition_variable blocker;

    std::vector<std::thread> threads;
    std::queue<m_task> task_queue;
    
    std::atomic<bool> open;
    std::atomic<bool> active;
    std::atomic<int> free_thread;

    void run();
    m_task fetch_task();

    public:
    
    ThreadPool(int thread_pool_size = 2);
    ~ThreadPool();

    /*
        push a task to thread pool
        task a function or callable object
        args arguments
        return task return type, can be accessed future
    */
    template<typename F, typename... Args>
    auto pushTask(F && task, Args &&... args) -> std::future<decltype(task(args...))>  {
        if(!open.load()) {
            /* throw exception */
        }

        auto new_task = std::make_shared<std::packaged_task<decltype(task(args...))()>>(
            std::bind(std::forward<F>(task),std::forward<Args>(args)...));

        std::unique_lock<std::mutex> task_queue_guard(task_access_mutex);
        task_queue.push(
            [new_task](){
                (*new_task)();
            }
        );
        task_queue_guard.unlock();
        blocker.notify_all();
        return new_task->get_future();
    }

    /*
        push a task to thread pool
        the task could be a member function
        the called object itself must be the first argument
    */
    template<typename F, typename... Args>
    auto pushMembTask(F && task, Args &&... args) {
        return pushTask(std::bind(std::forward<F>(task),std::forward<Args>(args)...));
    }

    /*
        activate the thread pool, thread start running, 
        the start status of a thread pool is active
    */
    void activate();

    /* deactivate it, the thread will stop running AFTER all the task done*/
    void deactivate();

    /*
        stop all the thread and DROP all the remained task in task queue 
        WARNNING!: which task(s) will be drop is unknown
    */
    void forceShutdown();

    /*
        start accept task pushes
        will be called when pool constructed
    */
    void startAccept();

    /* stop accept new tasks */
    void stopAccept();

    /* status data interface */
    bool isOpen() const;
    bool isAactivate() const;
};

#endif