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
    auto res = t.execute(muti_two, 2.5);
    t.execute(hello);
    // wait until finish threadpool
    t.join();
    // print result multi_two funtion
    std::cout << "result: " << res.get();
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

## Example
`example.cpp` is print pi number with 100 digits after point.

for run example :
```console
$ make run
g++ -Wall -pthread -std=c++17 -c example.cpp -o example.o
g++ -c -Wall -pthread -std=c++17 BigFloat/BigFloat.cc
g++ -Wall -pthread -std=c++17 example.o BigFloat.o -o example
./example
PI:
+3.1415926535897932384626433832795028841971693993751058209749445923078164062862089981395381067916552216
```
## REFERENCES
based on:
- https://www.sobyte.net/post/2022-05/design-a-thread-pool/#thread-pooling
- https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h

## Third Party
- [BigFloat](https://github.com/Mariotti94/BigFloat)

## TODO
- [ ] compatible with c++11 or lower
- [ ] compatible with c
- [ ] implement proccesspool
- [ ] implement for win32
