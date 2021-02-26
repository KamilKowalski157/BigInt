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

class BigInt
{

    static char hexLookupTable[16];
    static char conversionLookupTable[256];
    const static uint32_t endian32 = ((uint32_t)1 << (sizeof(uint32_t) * 8 - 1));

    const static uint64_t bigEndianMask = ((uint64_t)1 << (sizeof(uint64_t) * 8 - 1));

    mutable uint64_t buffer;

    bool sign = false;
    uint32_t *digits = nullptr;

    void leftShiftDigits(uint32_t n);
    void rightShiftDigits(uint32_t n);

    void deallocate();

    void mulAdd(const BigInt &a, uint64_t b);
    void mulSub(const BigInt &a, uint64_t b);

    void karatsuba(const BigInt &a, const BigInt &b, BigInt &buff1);
    void naiveMul(const BigInt &a, const BigInt &b);

    //void naiveDiv(const BigInt &a, const BigInt &b);

    friend std::ostream &operator<<(std::ostream &stream, const BigInt &b);
#ifdef __DEBUG__
    friend Tester;
#endif /*__DEBUG__*/

    /*digits(temp.digits + std::min(temp.n - 1, pos)),
                                                                       n(std::min((long)_size, digits - temp.digits)),
                                                                       owner(false),
                                                                       sign(temp.sign)*/
    BigInt(const BigInt &temp, unsigned int pos, unsigned int _size) : digits(temp.digits + std::min(temp.size, pos)), // Deprecated
                                                                       size(std::min((long)_size, temp.size - (digits - temp.digits))),
                                                                       sign(temp.sign)
    {
    }

public:
    const uint32_t size = 0;

    BigInt(uint32_t a = 1) : size(allocate(a)) { sign = 0; }
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
    //BigInt operator%=(const BigInt &b);
    BigInt &operator*=(const BigInt &b);
    BigInt &operator/=(const BigInt &b);
    BigInt &operator<<=(int shift);
    BigInt &operator>>=(int shift);
    BigInt &operator*=(const int32_t b);

    void sqrt(const BigInt &base);
    void pow(const BigInt &base, uint32_t exponent);
    void modExp(const BigInt &base, uint32_t exponent, const BigInt &modulus);

    bool operator<(const BigInt &b) const;
    bool operator>(const BigInt &b) const;
    bool operator==(const BigInt &b) const;
    bool operator!=(const BigInt &b) const;

    BigInt &operator=(const BigInt &b);
    BigInt &operator=(BigInt &&b);

    uint32_t operator[](int i) const
    {
        return digits[i * (i < size)] * (i >= 0) * (i < size) + (~0) * sign * (!(i < size || i < 0));
    }
    void computeInverse(const BigInt &a);

    std::string toHex() const;
    std::string toDec() const;
    std::string toBin() const;

    bool isNegative() const { return (digits != nullptr && (digits[size - 1] & endian32)); }
    void negate();
    bool abs();
    void clear();

    unsigned int getActualSize() const; // returns size without leading 0s (or 1s in case of negative complementation)

    void shiftLeft();  //Fast implementation for single shift
    void shiftRight(); // Fast implementation for single shift

    ~BigInt();
};

union Trickster
{
    BigInt bint;
    struct
    {
        uint64_t buffer;
        bool sign;
        uint32_t *digits;
        uint32_t size;
    } fields;
    Trickster(bool _sign, uint32_t *_digits, uint32_t _size) : fields{0,_sign,_digits,_size} {}
    ~Trickster() {}
};

std::ostream &operator<<(std::ostream &stream, const BigInt &b);

#endif /*BIGINT*/