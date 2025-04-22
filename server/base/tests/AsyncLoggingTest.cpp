//
// Created by 19948 on 2025/4/21.
//

#include <string>
#include <unistd.h>
#include <vector>
#include <memory>
#include <iostream>

#include "Logging.h"
#include "Thread.h"

using namespace std;

const int TestNum = 20;
const int ThreadCOUNt = 4;

void threadFunc() {
    for (int i = 0; i < TestNum; ++i)
    {
        LOG << i;
    }
}

void type_test()
{
    // 13 lines
    cout << "----------type test-----------" << endl;
    cout << "----------LOG 1-----------" << endl;
    LOG << 0;
    std::this_thread::sleep_for(std::chrono::microseconds(3000));
    cout << "----------LOG 2-----------" << endl;
    LOG << 1234567890123;
    std::this_thread::sleep_for(std::chrono::microseconds(3000));
    cout << "----------LOG 3-----------" << endl;
    LOG << 1.0f;
    std::this_thread::sleep_for(std::chrono::microseconds(3000));
}

void stressing_single_thread()
{
    // 100000 lines
    cout << "----------stressing test single thread-----------" << endl;
    for (int i = 0; i < 10; ++i)
    {
        cout << "LOG: " << i << " Line\n";
        LOG << i;
    }
}

void stressing_multi_threads(int threadNum = ThreadCOUNt)
{
    // threadNum * 100000 lines
    cout << "----------stressing test multi thread-----------" << endl;
    vector<shared_ptr<Thread>> vsp;
    for (int i = 0; i < threadNum; ++i)
    {
        shared_ptr<Thread> tmp(new Thread(threadFunc, "testFunc"));
        vsp.push_back(tmp);
    }
    for (int i = 0; i < threadNum; ++i)
    {
        vsp[i]->start();
    }
    sleep(3);
}

void other()
{
    // 1 line
    cout << "----------other test-----------" << endl;
    LOG << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
}

int main() {
    // ¹²500014ÐÐ
    // type_test();
    // sleep(3);

    stressing_single_thread();
    sleep(3);
    //
    // other();
    // sleep(3);
    //
    // stressing_multi_threads();
    // sleep(3);
    return 0;
}



