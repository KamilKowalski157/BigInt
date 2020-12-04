#ifndef TESTER
#define TESTER

#include <chrono>

class BigInt;
class Tester
{
    std::chrono::time_point<std::chrono::system_clock> timer1;
    void startTimer();
    double stopTimer();
public:
    void testAddition(unsigned int n = 1000);
    void testSubtraction(unsigned int n = 1000);
    void testDivision(unsigned int n = 1000);
    void testMultiplication(unsigned int n = 1000);
    void testComparision(unsigned int n = 1000);
};

#endif /*TESTER*/