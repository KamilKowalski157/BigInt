#ifndef BIGINT
#define BIGINT
#define __DEBUG__

#include <stdint.h>
#include <string>
#include <cstring>
#include <ostream>

//  TODO:
//- Optimize reallocation from existing bigint
//- Implement += /= -= *= operators for improved performance
//- Karatsuba multiplication with smart memory managemenet

class Tester;

class BigInt
{
    static uint32_t endianMask;
    uint32_t *digits = nullptr;
    mutable uint64_t buffer;
    unsigned int n = 0; //length of digits table
    unsigned int fullDigits = 0;
    
    bool isNegative() const { return (digits != nullptr && (digits[n - 1] & endianMask)); }
    void negate();
    bool abs();

    unsigned int getActualSize() const; // returns size without leading 0s (or 1s in case of negative complementation)

    void shiftLeft();  //Fast implementation for single shift
    void shiftRight(); // Fast implementation for single shift;

    void karatsuba(BigInt &a, BigInt &b);

    friend std::ostream &operator<<(std::ostream &stream, const BigInt &b);
#ifdef __DEBUG__
    friend Tester;
#endif /*__DEBUG__*/

    BigInt(uint32_t *_dig, unsigned int _size) : digits(_dig), n(_size){}

public:
    BigInt(int a = 0)
    {
        reallocate(1);
        digits[0] = a;
    }
    BigInt(const std::string &decStr);
    BigInt(BigInt &&b);
    BigInt(const BigInt &b);

    void reallocate(unsigned int target_size); // should return pointer and size, and be const for maximum efficiency (minimizing new calls)

    BigInt operator+(const BigInt &b) const;
    BigInt operator-(const BigInt &b) const;
    BigInt operator*(const BigInt &b) const;
    BigInt operator/(const BigInt &b) const;

    BigInt &operator+=(const BigInt &b);
    BigInt &operator-=(const BigInt &b);
    BigInt &operator*=(const BigInt &b);
    BigInt &operator/=(const BigInt &b);

    BigInt operator<<(int shift) const;
    BigInt operator>>(int shift) const;

    bool operator<(const BigInt &b) const;
    bool operator>(const BigInt &b) const;
    bool operator==(const BigInt &b) const;
    bool operator!=(const BigInt &b) const;

    BigInt &operator=(const BigInt &b);
    BigInt &operator=(BigInt &&b);
    //BigInt & operator()(const BigInt & b);

    //test functions (to be removed)
    std::string toHex() const;
    std::string toDec() const;
    std::string toBin() const;

    ~BigInt();
};

std::ostream &operator<<(std::ostream &stream, const BigInt &b);

#endif /*BIGINT*/