/*
    ExclusiveSharedLock.cpp Created 2021/6/5
    By: Ag2S
*/

#include "ExclusiveSharedLock.h"

ExclusiveSharedLock::ExclusiveSharedLock() = default;

ExclusiveSharedLock::~ExclusiveSharedLock() = default;

void ExclusiveSharedLock::SharedLock() {
    read_getter_lock.lock();
    if(lock_num++ == 0) {
        write_lock.lock();
    }
    read_getter_lock.unlock();
}

void ExclusiveSharedLock::SharedUnlock() {
    read_getter_lock.lock();
    if(--lock_num == 0) {
        write_lock.unlock();
    }
    read_getter_lock.unlock();
}

void ExclusiveSharedLock::ExclusiveLock() {
    write_lock.lock();
}

void ExclusiveSharedLock::ExclusiveUnlock() {
    write_lock.lock();
}