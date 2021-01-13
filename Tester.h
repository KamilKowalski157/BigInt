#ifndef TESTER
#define TESTER

#include <chrono>
#include <random>

class BigInt;
class Tester
{
    std::chrono::time_point<std::chrono::system_clock> timer1;
    void startTimer();
    double stopTimer();
    void generate(BigInt &a, int size, std::default_random_engine &eng);

public:
    unsigned int testAddition(unsigned int n = 1000);
    unsigned int testSubtraction(unsigned int n = 1000);
    unsigned int testAddSubtr(unsigned int n = 1000);
    unsigned int testShift(unsigned int n = 1000);
    unsigned int testDivision(unsigned int n = 1000);
    unsigned int testMultiplication(unsigned int n = 1000);
    unsigned int testComparision(unsigned int n = 1000);
    unsigned int testInversion(unsigned int n = 1000);
    void manual();
};

#endif /*TESTER*/