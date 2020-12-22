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
unsigned int Tester::testAddition(unsigned int n)
{
    unsigned int failures = 0;
    std::default_random_engine engine;
    auto dist = std::uniform_int_distribution(0, 999999);
    startTimer();
    for (int i = 0; i < n; i++)
    {
        auto x = dist(engine) - 500000;
        auto y = dist(engine) - 500000;
        BigInt a((uint32_t)x);
        BigInt b((uint32_t)y);
        a -= b;
        BigInt c(x - y);
        if (a != c)
        {
            failures++;
            std::cout << "Invalid result. For a =\t" << x << "\tand b =\t" << y << "\tgives\t" << a << " instead of\t" << c << std::endl;
        }
        else
        {
            //std::cout<<"Passed test for: a =\t"<<x<<"\tand b =\t"<<y<<"\twith result =\t"<<c<<std::endl;
        }
    }
    double time = stopTimer();
    std::cout << time << " milliseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " microseconds per trial" << std::endl;
    std::cout << failures << " / " << n << " trials failed" << std::endl;
    return failures;
}
unsigned int Tester::testSubtraction(unsigned int n)
{
    unsigned int failures = 0;
    std::default_random_engine engine;
    auto dist = std::uniform_int_distribution(0, 9999999);
    startTimer();
    for (int i = 0; i < n; i++)
    {
        auto x = dist(engine) - 5000000;
        auto y = dist(engine) - 5000000;
        BigInt a((uint32_t)x + y);
        a-= y;
        BigInt c((uint32_t)x);
        if (a != c)
        {
            failures++;
            std::cout << "Invalid result. For a =\t" << x + y << "\tand b =\t" << y << "\tgives\t" << c << " instead of\t" << (x) << std::endl;
        }
        else
        {
            //std::cout << "Passed test for: a =\t" << x + y << "\tand b =\t" << y << "\twith result =\t" << c << std::endl;
        }
    }
    double time = stopTimer();
    std::cout << time << " milliseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " microseconds per trial" << std::endl;
    std::cout << failures << " / " << n << " trials failed" << std::endl;
    return failures;
}
unsigned int Tester::testAddSubtr(unsigned int n)
{
    unsigned int failures = 0;
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);
    auto dist = std::uniform_int_distribution(0, 999999);

    std::cout << "generated on seed: " << seed << std::endl;

    startTimer();
    for (int i = 0; i < n; i++)
    {
        BigInt a[2];
        a[0].reallocate(10);
        a[1].reallocate(10);
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                a[k].digits[j] = dist(engine);
            }
        }
        if ((a[0] - a[1]) + a[1] != a[0])
        {
            std::cout << "Trial failed for:\t" << a[0] << " and\t" << a[1] << "with sum equal:\t" << (a[0] + a[1]) << " and difference equal:\t" << ((a[0] - a[1]) + a[1]) << std::endl;
            failures++;
        }
        else
        {
            std::cout << "Trial passed for:\t" << a[0] << " and\t" << a[1] << " with sum of:\t" << (a[0] + a[1]) << std::endl;
        }
    }
    double time = stopTimer();
    std::cout << time << " milliseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " milliseconds per trial" << std::endl;
    std::cout << failures << " / " << n << " trials failed" << std::endl;
    return failures;
}
unsigned int Tester::testMultiplication(unsigned int n)
{
    unsigned int failures = 0;
    std::default_random_engine engine;
    auto dist = std::uniform_int_distribution(0, 99999);
    startTimer();
    for (int i = 0; i < n; i++)
    {
        auto x = dist(engine) - 50000;
        auto y = dist(engine) - 50000;
        BigInt a((uint32_t)x);
        BigInt b((uint32_t)y);
        BigInt c((uint32_t)x * y);
        if ((a * b) != c)
        {
            failures++;
            std::cout << "Invalid result. For a =\t" << x << "\tand b =\t" << y << "\tgives\t" << c << " instead of\t" << (x * y) << std::endl;
        }
        else
        {
            std::cout << "Passed test for: a =\t" << x << "\tand b =\t" << y << "\twith result =\t" << c << std::endl;
        }
    }
    double time = stopTimer();
    std::cout << time << " milliseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " milliseconds per trial" << std::endl;
    std::cout << failures << " / " << n << " trials failed" << std::endl;
    return failures;
}
unsigned int Tester::testShift(unsigned int n) // Manual
{
    unsigned int failures = 0;
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);
    auto dist = std::uniform_int_distribution(0, 9999999);
    auto dist2 = std::uniform_int_distribution(0, 9999999);

    std::cout << "generated on seed: " << seed << std::endl;

    startTimer();
    for (int i = 0; i < n; i++)
    {
        int x = dist(engine);
        int y = dist2(engine);
        uint32_t * ptr = new uint32_t[2];
        ptr[0] = x;
        ptr[1] = y;
        BigInt a(ptr,2);
        for(int i = 0;i<20;i++)
        {
            std::cout << (a<<(-i)).toBin() << std::endl;
            std::cin.get();
        }
        std::cout << "fin" << std::endl;
    }
    double time = stopTimer();
    std::cout << time << " milliseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " milliseconds per trial" << std::endl;
    std::cout << failures << " / " << n << " trials failed" << std::endl;
    return failures;
}
unsigned int Tester::testDivision(unsigned int n)
{
    unsigned int failures = 0;
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);
    auto dist = std::uniform_int_distribution(0, 999999);
    auto dist2 = std::uniform_int_distribution(0, 9999);

    std::cout << "generated on seed: " << seed << std::endl;

    startTimer();
    for (int i = 0; i < n; i++)
    {
        int x = dist(engine);
        int y = dist2(engine);
        if(y==0){continue;}
        BigInt a((uint32_t)x);
        BigInt b((uint32_t)y);
        BigInt c((uint32_t)(x / y));

        if ((a / b) != c)
        {
            std::cout << "Trial failed for:\t" << x << " and\t" << y << " with result equal:\t" << (a / b) << " and answer equal:\t" << c << std::endl;
            failures++;
        }
        else
        {
            //std::cout<<"Trial passed for:\t"<<a<<" and\t"<<y<<" with result of:\t"<<b<<" or:\t"<<(x<<y)<<std::endl;
        }
    }
    double time = stopTimer();
    std::cout << time << " milliseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " milliseconds per trial" << std::endl;
    std::cout << failures << " / " << n << " trials failed" << std::endl;
    return failures;
}