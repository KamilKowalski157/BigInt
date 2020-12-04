#include "Tester.h"
#include "BigInt.h"
#include <random>
#include <iostream>

void Tester::startTimer()
{
    timer1 = std::chrono::high_resolution_clock::now();
}
double Tester::stopTimer()
{
    auto timer2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(timer2 - timer1).count();
}
void Tester::testAddition(unsigned int n)
{
    std::default_random_engine engine;
    auto dist = std::uniform_int_distribution(0,999999);
    for (int i = 0; i < n; i++)
    {
        auto x = dist(engine);
        auto y = dist(engine);
        BigInt a(x);
        BigInt b(y);
        BigInt c(a+b);
        if (a + b != c)
        {
            std::cout<<"Invalid result. For a = "<<x<<" and b = "<<y<<" gives "<<c<<" instead of "<<(x+y)<<std::endl;
        }
        else
        {
            std::cout<<"Passed test for: a = "<<x<<" and b = "<<y<<" with result = "<<c<<std::endl;
        }
    }
}