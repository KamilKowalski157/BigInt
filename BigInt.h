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
    const static uint64_t bigEndianMask = ((uint64_t)1 << (sizeof(uint64_t) * 8 - 1));

    mutable uint64_t buffer;

    const bool owner;
    uint32_t *digits = nullptr;
    unsigned int n = 0; //length of digits table
    bool sign = false;

    bool isNegative() const { return (digits != nullptr && (digits[n - 1] & endianMask)); }
    void negate();
    bool abs();
    void clear();

    unsigned int getActualSize() const; // returns size without leading 0s (or 1s in case of negative complementation)

    void shiftLeft();  //Fast implementation for single shift
    void shiftRight(); // Fast implementation for single shift
    BigInt computeInverse(unsigned int k) const;

    void deallocate();

    void mulAdd(const BigInt &a, uint64_t b);

    void karatsuba(const BigInt &a, const BigInt &b, BigInt &buff1);

    friend std::ostream &operator<<(std::ostream &stream, const BigInt &b);
#ifdef __DEBUG__
    friend Tester;
#endif /*__DEBUG__*/

    /*digits(temp.digits + std::min(temp.n - 1, pos)),
                                                                       n(std::min((long)_size, digits - temp.digits)),
                                                                       owner(false),
                                                                       sign(temp.sign)*/
    BigInt(const BigInt &temp, unsigned int pos, unsigned int _size) : digits(temp.digits + std::min(temp.n, pos)),
                                                                       n(std::min((long)_size, temp.n - (digits - temp.digits))),
                                                                       owner(false),
                                                                       sign(temp.sign)
    {
    }

public:
    BigInt(int a = 0) : owner(true)
    {
        reallocate(1);
        digits[0] = a;
        sign = isNegative();
    }
    BigInt(const std::string &decStr);
    BigInt(BigInt &&b);
    BigInt(const BigInt &b);

    bool reallocate(unsigned int target_size); // should return pointer and size, and be const for maximum efficiency (minimizing new calls)

    BigInt operator+(const BigInt &b) const;
    BigInt operator-(const BigInt &b) const;
    BigInt operator*(const BigInt &b) const;
    BigInt operator*(int32_t b) const;
    BigInt operator/(const BigInt &b) const;
    BigInt operator%(const BigInt &b) const;

    BigInt &operator+=(const BigInt &b);
    BigInt &operator-=(const BigInt &b);
    BigInt &operator*=(const BigInt &b);
    BigInt &operator*=(const int32_t b);
    BigInt &operator/=(const BigInt &b);

    BigInt operator<<(int shift) const;
    BigInt operator>>(int shift) const;

    bool operator<(const BigInt &b) const;
    bool operator>(const BigInt &b) const;
    bool operator==(const BigInt &b) const;
    bool operator!=(const BigInt &b) const;

    BigInt &operator=(const BigInt &b);
    BigInt &operator=(BigInt &&b);

    uint32_t operator[](int i) const
    {
        return digits[i * (i < n)] * (i >= 0) * (i < n) + (~0) * sign * (!(i < n));
    }
    //BigInt & operator()(const BigInt & b);

    //test functions (to be removed)
    std::string toHex() const;
    std::string toDec() const;
    std::string toBin() const;

    ~BigInt();
};

std::ostream &operator<<(std::ostream &stream, const BigInt &b);

#endif /*BIGINT*/