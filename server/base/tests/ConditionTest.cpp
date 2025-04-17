//
// Created by 19948 on 2025/4/17.
//

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <chrono>
#include <cassert>

#include "Condition.h"


MutexLock mtx;
Condition condition(mtx);
bool ready = false;



int main() {

}