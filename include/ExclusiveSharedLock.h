/*
    ExclusiveSharedLock.h Created 2021/6/4
    By: Ag2s
*/

#ifndef EXCLUSIZESHAREDLOCK_H
#define EXCLUSIZESHAREDLOCK_H


#include <mutex>

class ExclusiveSharedLock {
    std::mutex read_getter_lock;
    std::mutex write_lock;
    int lock_num;

    public:

    ExclusiveSharedLock();
    ~ExclusiveSharedLock();

    void SharedLock();
    void SharedUnlock();

    void ExclusiveLock();
    void ExclusiveUnlock();
};

#endif