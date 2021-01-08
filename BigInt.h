#ifndef BIGINT
#define BIGINT
#define __DEBUG__

#include <stdint.h>
#include <string>
#include <cstring>
#include <ostream>

//  TODO:
//- Optimize reallocation from existing bigint
//- Implement += /= -= *= operators for improved performance - done
//- Karatsuba multiplication with smart memory managemenet - done
//- add end pointer and iterator pointer to speed up

class Tester;

class BigInt
{
    static uint32_t endianMask;

    mutable uint64_t buffer;

    const bool owner = true;

    uint32_t *digits = new uint32_t[2];
    uint32_t *iterator = digits;
    uint32_t *end = digits;

    unsigned int n = 0; //length of digits table
    uint32_t *sign = digits+1;

    void negate();
    bool abs();
    void clear();

    unsigned int getActualSize() const;                   // returns size without leading 0s (or 1s in case of negative complementation)
    uint32_t isNegative() const { return ((*sign) & 1); } //types faster... also, inline

    // internal functions optimized for specific operations...
    void shiftLeft();  //Fast implementation for single shift
    void shiftRight(); // Fast implementation for single shift
    void addMul(const BigInt &a, const BigInt &b);
    void subMul(const BigInt &a, const BigInt &b);
    uint64_t carryAdd(const BigInt &a, uint64_t buf);
    //uint64_t carrySub(const BigInt &a, uint64_t buf);

    void deallocate();

    void karatsuba(BigInt &a, BigInt &b, BigInt &result, BigInt &buff1);

    BigInt computeInverse() const;

    friend std::ostream &operator<<(std::ostream &stream, const BigInt &b);
#ifdef __DEBUG__
    friend Tester;
#endif /*__DEBUG__*/

    BigInt(const BigInt &temp, unsigned int pos, unsigned int _size) :  owner(false), digits(temp.digits + std::min(pos, temp.n - 1)),
                                                                       n(std::min(temp.n - pos, _size)), sign(new uint32_t(*temp.sign))
    {
    }

public:
    BigInt(int a = 0) : owner(true)
    {
        reallocate(1);
        digits[0] = a;
        *sign = (a < 0);
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
    BigInt &operator*=(uint32_t b);
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
        return digits[i * (i < n)] * (i >= 0) * (i < n) + (~0) * isNegative() * (!(i < n));
    }
    //BigInt & operator()(const BigInt & b);

    //test functions (to be removed)
    std::string toHex() const;
    std::string toDec() const; //Double and dubble ???
    std::string toBin() const;

    ~BigInt();
};

std::ostream &operator<<(std::ostream &stream, const BigInt &b);

#endif /*BIGINT*/