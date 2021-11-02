#include "threadmanager.h"

ThreadManager::ThreadManager(){
    threadPool.resize(1);
}

ThreadManager::ThreadManager(int threadCount){
    threadPool.resize(threadCount);
}

ThreadManager::~ThreadManager(){
    threadPool.clear();
}

ThreadManager::executeFunction(){

}
