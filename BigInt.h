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
class BigInt;

//class BufferStack

class BigInt
{
    const static uint32_t endian32 = ((uint32_t)1 << (sizeof(uint32_t) * 8 -1));
    
    const static uint64_t bigEndianMask = ((uint64_t)1 << (sizeof(uint64_t) * 8 - 1));

    mutable uint64_t buffer;

    bool sign = false;
    uint32_t *digits = nullptr;
    const uint32_t size = 0;

    bool isNegative() const { return (digits != nullptr && (digits[size - 1] & endian32)); }
    void negate();
    bool abs();
    void clear();

    unsigned int getActualSize() const; // returns size without leading 0s (or 1s in case of negative complementation)

    void shiftLeft();  //Fast implementation for single shift
    void shiftRight(); // Fast implementation for single shift
    BigInt computeInverse(unsigned int k) const;

    void deallocate();

    void mulAdd(const BigInt &a, uint64_t b);
    void mulSub(const BigInt &a, uint64_t b);

    void karatsuba(const BigInt &a, const BigInt &b, BigInt &buff1);

    friend std::ostream &operator<<(std::ostream &stream, const BigInt &b);
#ifdef __DEBUG__
    friend Tester;
#endif /*__DEBUG__*/

    /*digits(temp.digits + std::min(temp.n - 1, pos)),
                                                                       n(std::min((long)_size, digits - temp.digits)),
                                                                       owner(false),
                                                                       sign(temp.sign)*/
    BigInt(const BigInt &temp, unsigned int pos, unsigned int _size) : digits(temp.digits + std::min(temp.size, pos)),
                                                                       size(std::min((long)_size, temp.size - (digits - temp.digits))),
                                                                       sign(temp.sign)
    {
    }

public:
    BigInt(int a = 1) : size(allocate(a)){sign = 0;}
    BigInt(const std::string &decStr);
    BigInt(BigInt &&b);
    BigInt(const BigInt &b);

    uint32_t allocate(unsigned int target_size);

    BigInt operator+(const BigInt &b) const;
    BigInt operator-(const BigInt &b) const;
    BigInt operator%(const BigInt &b) const;
    BigInt operator*(const BigInt &b) const;
    BigInt operator/(const BigInt &b) const;
    BigInt operator<<(int shift) const;
    BigInt operator>>(int shift) const;
    BigInt operator*(int32_t b) const;

    BigInt &operator+=(const BigInt &b);
    BigInt &operator-=(const BigInt &b);
    BigInt operator%=(const BigInt &b);
    BigInt &operator*=(const BigInt &b);
    BigInt &operator/=(const BigInt &b);
    BigInt& operator<<=(int shift);
    BigInt& operator>>=(int shift);
    BigInt &operator*=(const int32_t b);



    bool operator<(const BigInt &b) const;
    bool operator>(const BigInt &b) const;
    bool operator==(const BigInt &b) const;
    bool operator!=(const BigInt &b) const;

    BigInt &operator=(const BigInt &b);
    BigInt &operator=(BigInt &&b);

    uint32_t operator[](int i) const
    {
        return digits[i * (i < size)] * (i >= 0) * (i < size) + (~0) * sign * (!(i < size));
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