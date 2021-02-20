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

    BigInt a(size);
    BigInt b(size);
    BigInt c(size);
    double time;
    for (int i = 0; i < n; i++)
    {
        generate(a, size);
        generate(b, size);
        startTimer();
        c = a - b;
        time += stopTimer();
        if ((c + b) != a)
        {
            failures++;
            std::cout << "case: " << i << " test failed for a:" << a << "\n and b: " << b << std::endl;
        }
    }
    //double time = stopTimer();
    std::cout << time << " microseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " microseconds per trial" << std::endl;
    std::cout << (n - failures) << " / " << n << " trials passed" << std::endl;
    return failures;
}

unsigned int Tester::testShift(unsigned int n, unsigned int size) // Manual
{
    unsigned int failures = 0;
    startTimer();

    std::uniform_int_distribution dist(0, (int)size - 1);

    BigInt a(size * 2);
    BigInt b(size * 2);
    unsigned int shift;

    for (int i = 0; i < n; i++)
    {
        shift = dist(engine);
        a.clear();
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

    BigInt a(size);
    BigInt b(size);
    BigInt c(size * 2);
    BigInt d(size * 2);
    BigInt f(size * 2);
    auto dist = std::uniform_int_distribution(1U, size - 1);
    unsigned int size1;
    double time = 0;
    for (int i = 0; i < n; i++)
    {
        size1 = dist(engine);
        generate(a, size);
        generate(b, size);
        //c.naiveMul(a,b);
        c = a;
        //c.naiveMul(a,b);
        c = c*b;
        //std::cout << "a: " << a << " b: " << b << " c: " << c << " d(c/b): " << d << std::endl;
        //continue;
        startTimer();
        //c = c*b;
        //c.karatsuba(a, b, f);
        d = c/b;
        //d.naiveDiv(c,b);
        time += stopTimer();
        //continue;

        if (d != a)
        {
            //std::cout << "case: " << i << " test failed for a: " << a << "\n and b: " << b << std::endl;
            //std::cout << "c: " << c << std::endl;
            //std::cout << "d: " << d << std::endl;
            failures++;
            continue;
        }
        //std::cout << "a: " << a << " b: " << b << " c: " << c << " d(c/b): " << d << std::endl;
    }
    std::cout << time << " microseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " microseconds per trial" << std::endl;
    std::cout << (n - failures) << " / " << n << " trials passed" << std::endl;
    return failures;
}

void Tester::manual()
{
    BigInt y("");
    std::cout << y << std::endl;
}
void Tester::generate(BigInt &a, int size)
{
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
    BigInt c;
    BigInt d;
    startTimer();
    for (int i = 0; i < n; i++)
    {
        generate(a, size);
        generate(b, size / 2);
        c = a / b;
        d = a % b;
        if (c * b + d != a)
        {
            std::cout << "case: " << i << " test failed for a: " << a << "\n and b: " << b << std::endl;
            std::cout << "c: " << c << std::endl;
            std::cout << "d: " << d << std::endl;
            failures++;
        }
    }
    std::cout << std::endl;
    double time = stopTimer();
    std::cout << time << " microseconds elapsed. Executed " << n << " trials, which gives average of " << time / n << " microseconds per trial" << std::endl;
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
    else if (!strcmp(argv[0], "mod"))
    {
        testInversion(n, size);
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