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
    auto dist = std::uniform_int_distribution(0,999999);
    startTimer();
    for (int i = 0; i < n; i++)
    {
        auto x = dist(engine)-500000;
        auto y = dist(engine)-500000;
        BigInt a((uint32_t)x);
        BigInt b((uint32_t)y);
        BigInt c((a+b));
        if (a + b != c)
        {
            failures++;
            std::cout<<"Invalid result. For a =\t"<<x<<"\tand b =\t"<<y<<"\tgives\t"<<c<<" instead of\t"<<(x+y)<<std::endl;
        }
        else
        {
            std::cout<<"Passed test for: a =\t"<<x<<"\tand b =\t"<<y<<"\twith result =\t"<<c<<std::endl;
        }
    }
    double time = stopTimer();
    std::cout<<time<<" milliseconds elapsed. Executed "<<n<<" trials, which gives average of "<<time/n<< " milliseconds per trial"<<std::endl;
    std::cout<<failures<<" / "<<n<<" trials failed"<<std::endl;
    return failures;
}
unsigned int Tester::testSubtraction(unsigned int n)
{
    unsigned int failures = 0;
    std::default_random_engine engine;
    auto dist = std::uniform_int_distribution(0,999999);
    startTimer();
    for (int i = 0; i < n; i++)
    {
        auto x = dist(engine)-500000;
        auto y = dist(engine)-500000;
        BigInt a((uint32_t)x+y);
        BigInt b((uint32_t)y);
        BigInt c((uint32_t)x);
        if ((a-b) != c)
        {
            failures++;
            std::cout<<"Invalid result. For a =\t"<<x+y<<"\tand b =\t"<<y<<"\tgives\t"<<c<<" instead of\t"<<(x)<<std::endl;
        }
        else
        {
            std::cout<<"Passed test for: a =\t"<<x+y<<"\tand b =\t"<<y<<"\twith result =\t"<<c<<std::endl;
        }
    }
    double time = stopTimer();
    std::cout<<time<<" milliseconds elapsed. Executed "<<n<<" trials, which gives average of "<<time/n<< " milliseconds per trial"<<std::endl;
    std::cout<<failures<<" / "<<n<<" trials failed"<<std::endl;
    return failures;
}
unsigned int Tester::testAddSubtr(unsigned int n)
{
    unsigned int failures = 0;
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);
    auto dist = std::uniform_int_distribution(0,999999);

    std::cout<<"generated on seed: "<<seed<<std::endl;

    startTimer();
    for (int i = 0; i < n; i++)
    {
        BigInt a[2];
        for(int j = 0;j<10;j++)
        {
            for(int k = 0;k<2;k++)
            {
                a[k].reallocate(j+1);
                a[k].digits[j] = dist(engine);
            }
        }
        if((a[0]-a[1])+a[1] != a[0])
        {
            std::cout<<"Trial failed for:\t"<<a[0]<<" and\t"<<a[1]<<"with sum equal:\t"<<(a[0]+a[1])<<" and difference equal:\t"<<((a[0]-a[1])+a[1])<<std::endl;
        }
        else
        {
            std::cout<<"Trial passed for:\t"<<a[0]<<" and\t"<<a[1]<<" with sum of:\t"<<(a[0]+a[1])<<std::endl;
        }
        
    }
    double time = stopTimer();
    std::cout<<time<<" milliseconds elapsed. Executed "<<n<<" trials, which gives average of "<<time/n<< " milliseconds per trial"<<std::endl;
    std::cout<<failures<<" / "<<n<<" trials failed"<<std::endl;
    return failures;
}