#include <iostream>

using namespace std;

int add(int a, int b) {
    return a + b;
}

int main() {
    int a, b;
    a = 5;
    b = 10;
    cout << "The sum is: " << add(a, b) << endl;
    return 0;
}