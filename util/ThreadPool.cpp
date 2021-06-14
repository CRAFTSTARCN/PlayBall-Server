/*
    ThreadPool.cpp Created 2021/5/17
    By Ag2S
*/

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int thread_pool_size) {
    active.store(true);
    open.store(true);
    for(int i=0; i<thread_pool_size; ++i) {
        threads.push_back(std::thread(run,this));
    }
    free_thread.store(thread_pool_size);
    startAccept();
}

ThreadPool::~ThreadPool() {
    deactivate();
}

void ThreadPool::activate() {
    std::unique_lock<std::mutex> lock(task_access_mutex);
    if(active.load()) {
        return;
    }
    for(auto& thd : threads) {
        thd = std::thread(run,this);
    }
    active.store(true);
    open.store(true);
    lock.unlock();
    blocker.notify_all();
}

void ThreadPool::deactivate() {
    std::unique_lock<std::mutex> lock(task_access_mutex);
    if(!active.load()) {
        return;
    }
    open.store(false);
    active.store(false);
    lock.unlock();
    blocker.notify_all();
    for(auto& thd : threads) if(thd.joinable()) thd.join();
}

void ThreadPool::forceShutdown() {
    std::unique_lock<std::mutex> drop_task_lock(task_access_mutex);
    while(!task_queue.empty()) task_queue.pop();
    drop_task_lock.unlock();
    deactivate();
}

void ThreadPool::startAccept() {
    std::lock_guard<std::mutex> sync_gard(task_access_mutex);
    if(active.load())
        open.store(true);
}

void ThreadPool::stopAccept() {
    active.store(false);
}

bool ThreadPool::isOpen() const {
    return open.load();
}

bool ThreadPool::isAactivate() const {
    return active.load();
}

void ThreadPool::run() {
    while(true) {
        if(m_task task = fetch_task()) {
            task();
        } else {
            return;
        }
    } 
}

ThreadPool::m_task ThreadPool::fetch_task() {
    std::unique_lock<std::mutex> task_access_lock(task_access_mutex);
    blocker.wait(task_access_lock,[this](){return !this->task_queue.empty() || !this->active.load();});
    if(task_queue.empty()) return nullptr;
    auto task = task_queue.front();
    task_queue.pop();
    return task;
}