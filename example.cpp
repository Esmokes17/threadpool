#include <iostream>
#include <mutex>
#include <thread>

#include "threadpool.hpp"
#include "BigFloat/BigFloat.h"

std::shared_mutex global_mutex;
BigFloat res(0);

#define PREC 100 + 2

// formula calculate PI: https://en.wikipedia.org/wiki/Bailey-Borwein-Plouffe_formula
BigFloat formula(int i) {
    BigFloat res;
    res = BigFloat::PrecDiv(BigFloat(1) ,BigFloat::Power(BigFloat(16), BigFloat(i)), PREC);
    BigFloat inside1 = BigFloat::PrecDiv( BigFloat(4) , BigFloat(8 * i + 1), PREC);
    BigFloat inside2 = BigFloat::PrecDiv( BigFloat(-2), BigFloat(8 * i + 4), PREC);
    BigFloat inside3 = BigFloat::PrecDiv( BigFloat(-1), BigFloat(8 * i + 5), PREC);
    BigFloat inside4 = BigFloat::PrecDiv( BigFloat(-1), BigFloat(8 * i + 6), PREC);
    return (res * (inside1 + inside2 + inside3 + inside4));
}

void calculate_formula(int count) {
    std::shared_lock<std::shared_mutex> lock(global_mutex);
    res = res + formula(count);
}

int main() {
    ThreadPool t(5);
    for (int i = 0 ; i < PREC ; i++)
        t.execute(calculate_formula, i);
    t.join();

    res.SetPrecision(PREC-2);
    std::cout << "PI:\n" << res << std::endl;

    return 0;
}
