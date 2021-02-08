#include "Tester.h"
#include "BigInt.h"
#include <iostream>
#include <cstring>

int toInt(const char *str)
{
    int res = 0;
    bool sign = ((*str) == '-');
    while (*str != '\0')
    {
        res *= 10;
        if (*str < '0' || *str > '9')
        {
            return 0;
        } // Invalid characters
        res += (*str - '0');
        str++;
    }
    return (1 - 2 * sign) * res;
}

void Tester::startTimer()
{
    timer1 = std::chrono::high_resolution_clock::now();
}
double Tester::stopTimer()
{
    auto timer2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(timer2 - timer1).count();
}

unsigned int Tester::testAddSubtr(unsigned int n, unsigned int size)
{
    unsigned int failures = 0;
    startTimer();

    BigInt a;
    BigInt b;
    BigInt c;

    for (int i = 0; i < n; i++)
    {
        generate(a, size);
        generate(b, size);
        c = a - b;
        if ((c + b) != a)
        {
            failures++;
            std::cout << "case: " << i << " test failed for a:" << a << "\n and b: " << b << std::endl;
        }
    }
    double time = stopTimer();
    std::cout << time << " microseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " microseconds per trial" << std::endl;
    std::cout << (n - failures) << " / " << n << " trials passed" << std::endl;
    return failures;
}

unsigned int Tester::testShift(unsigned int n, unsigned int size) // Manual
{
    unsigned int failures = 0;
    startTimer();

    std::uniform_int_distribution dist(0, (int)size - 1);

    BigInt a;
    BigInt b;
    a.reallocate(size * 2);
    unsigned int shift;

    for (int i = 0; i < n; i++)
    {
        shift = dist(engine);
        generate(a, size);

        b = a;

        a = (a << shift);
        a = (a >> shift);

        if (a != b)
        {
            std::cout << "case: " << i << " test failed for a:" << b.toBin() << "\n and shift: " << shift << std::endl;
            std::cout << "a: " << a.toBin() << std::endl;
            failures++;
        }
    }
    double time = stopTimer();
    std::cout << time << " microseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " microseconds per trial" << std::endl;
    std::cout << (n - failures) << " / " << n << " trials passed" << std::endl;
    return failures;
}
unsigned int Tester::testMulDiv(unsigned int n, unsigned int size)
{
    unsigned int failures = 0;
    startTimer();

    BigInt a;
    BigInt b;
    BigInt c;
    c.reallocate(size * 2);
    BigInt d;
    d.reallocate(size * 2);

    for (int i = 0; i < n; i++)
    {
        generate(a, size);
        generate(b, size);
        if(i!=1){continue;}

        c = b;
        c = c * a;
        d = c / b;
        if (d != a)
        {
            std::cout << "case: " << i << " test failed for a: " << a << "\n and b: " << b << std::endl;
            std::cout << "c: " << c << std::endl;
            std::cout << "d: " << d << std::endl;
            failures++;
        }
    }
    double time = stopTimer();
    std::cout << time << " milliseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " milliseconds per trial" << std::endl;
    std::cout << (n - failures) << " / " << n << " trials passed" << std::endl;
    return failures;
}

void Tester::manual()
{
    BigInt y;
    generate(y,512);
    std::cout<<"y_2: "<<y.toBin()<<std::endl;
    std::cout<<"y_10: "<<y<<std::endl;
}
void Tester::generate(BigInt &a, int size)
{
    a.reallocate(size);
    auto dist = std::uniform_int_distribution(0, 1);
    for (int j = 0; j < size; j++)
    {
        for (int i = 0; i < 32; i++)
        {
            a.digits[j] += dist(engine);
            a.digits[j] = (a.digits[j] << 1);
        }
    }
}
unsigned int Tester::testInversion(unsigned int n, unsigned int size)
{
    unsigned int failures = 0;

    BigInt a;
    BigInt b;
    startTimer();
    for (int i = 0; i < n; i++)
    {
        std::cout << "trial " << i << "/" << n << "\r" << std::flush;
        generate(a, 128);
        generate(b, 128);
        if (!(b > a))
        {
            i--;
            continue;
        }
        BigInt y = (b.computeInverse(129));
        //((y*a)>>(129*32));
        if (a / b != ((y * a) >> (129 * 32)))
        {
            failures++;
            std::cout << "Test failed for a: " << a << std::endl;
            std::cout << "b: " << b << std::endl;
            std::cout << "inverse: " << y << std::endl;
            std::cout << "a/b : " << (a / b) << std::endl;
            std::cout << "a x 1/b: " << a * y << std::endl;
            std::cout << "1: " << b * y << std::endl;
            std::cin.get();
        }
        else
        {
            //std::cout<<(a/b)<<std::endl;
        }
    }
    std::cout << std::endl;
    double time = stopTimer();
    std::cout << time << " milliseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " milliseconds per trial" << std::endl;
    std::cout << (n - failures) << " / " << n << " trials passed" << std::endl;
    return failures;
}
void Tester::pickYourFighter(char **argv, int argc)
{
    if (argc < 1 || argc > 3)
    {
        std::cerr << "error: invalid argument count" << std::endl;
        return;
    }
    unsigned int n = 1000;
    unsigned int size = 128;
    if (argc > 1)
    {
        n = toInt(argv[1]);
    }
    if (argc > 2)
    {
        size = toInt(argv[2]);
    }

    std::cout << "executing " << argv[0] << " test for n = " << n << " and size = " << size << std::endl;

    if (!strcmp(argv[0], "a&s"))
    {
        testAddSubtr(n, size);
    }
    else if (!strcmp(argv[0], "m&d"))
    {
        testMulDiv(n, size); // Can be indepedent of division ???
    }
    else if (!strcmp(argv[0], "shf"))
    {
        testShift(n, size);
    }
    else if (!strcmp(argv[0], "man"))
    {
        manual();
    }
    else
    {
        std::cerr << "error: invalid argument" << std::endl;
    }
    return;
}