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

// void threadFunc() {
//     for (int i = 0; i < 100000; ++i)
//     {
//         LOGXXX << i;
//     }
// }

void type_test()
{
    // 13 lines
    cout << "----------type test-----------" << endl;
    LOG << 0;
    LOG << 1234567890123;
    LOG << 1.0f;
    LOG << 3.1415926;
    LOG << (short) 1;
    LOG << (long long) 1;
    LOG << (unsigned int) 1;
    LOG << (unsigned long) 1;
    LOG << (long double) 1.6555556;
    LOG << (unsigned long long) 1;
    LOG << 'c';
    LOG << "abcdefg";
    LOG << string("This is a string");
}

int main() {
    type_test();
}



