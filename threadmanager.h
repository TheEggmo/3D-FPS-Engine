#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <thread>
#include <vector>

using namespace std;

class ThreadManager{
private:
    vector<thread> threadPool;
public:
    ThreadManager();
    ThreadManager(int threadCount);
    ~ThreadManager();
    executeFunction();
    executeFunctionWait();
};

#endif // THREADMANAGER_H
