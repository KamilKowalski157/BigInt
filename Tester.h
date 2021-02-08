#ifndef TESTER
#define TESTER

#include <chrono>
#include <random>

class BigInt;
class Tester
{
    std::chrono::time_point<std::chrono::system_clock> timer1;
    std::default_random_engine engine;

    void startTimer();
    double stopTimer();
    void generate(BigInt &a, int size);

public:
    unsigned int testAddSubtr(unsigned int n = 1000,unsigned int size = 128);
    unsigned int testShift(unsigned int n = 1000,unsigned int size = 128);
    unsigned int testMulDiv(unsigned int n = 1000,unsigned int size = 128);
    unsigned int testComparision(unsigned int n = 1000,unsigned int size = 128);
    unsigned int testInversion(unsigned int n = 1000,unsigned int size = 128);
    void pickYourFighter(char ** args,int argc);
    void manual();

    Tester() : /*engine(0){}*/engine(std::chrono::system_clock::now().time_since_epoch().count()){}
};

#endif /*TESTER*/