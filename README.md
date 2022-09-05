# Thread pool in c++
this is a simple thread pool for c++=17 or higher.

## usage
make a file `threadpool.cpp` that contains:
```cpp
#include <iostream>

#include "threadpool.hpp"

// two different functions for execute
void hello() { std::cout << "Hello, World!" << std::endl; }
double multi_two(double a) { return 2 * a; }

int main() {
    // make threadpool
    ThreadPool t(2);
    // execute functions
    auto a = t.execute(muti_two, 2.5);
    t.execute(hello);
    // wait for finish the jobs
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // finish threadpool
    t.terminate();
    // print result multi_two funtion
    std::cout << "result: " << a.get();
    return 0;
}

```

then in teminal:

```console
$ g++ -Wall -pthread -std=c++17 -c threadpool.cpp -o threadpool.o
$ g++ -Wall -pthread -std=c++17 threadpool.o -o threadpool
$ ./threadpool
result multi function: 5
```

## REFRENCES
based on:
- https://www.sobyte.net/post/2022-05/design-a-thread-pool/#thread-pooling
- https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h

## TODO
- [ ] compatible with c++11 or lower
- [ ] compatible with c
- [ ] implement proccesspool
- [ ] implement for win32
