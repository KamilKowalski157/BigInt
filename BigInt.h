#ifndef BIGINT
#define BIGINT

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
    unsigned int n = 0;//length of digits table
    unsigned int fullDigits = 0;

    void reallocate(unsigned int target_size); // should return pointer and size, and be const for maximum efficiency (minimizing new calls)
    bool isNegative() const {return (digits!=nullptr&&(digits[n-1]&endianMask));}
    unsigned int getActualSize() const;// returns size without leading 0s (or 1s in case of negative complementation)
    void negate();
    bool abs();
    void shiftLeft(); //Fast implementation for single shift
    void shiftRight(); // Fast implementation for single shift;
    void karatsuba(BigInt & a,BigInt & b);

    friend std::ostream & operator<<(std::ostream & stream,const BigInt & b);
    friend Tester;
public:
    BigInt(){reallocate(1);digits[0] = 0;}
    template<typename... Args>
    BigInt(Args... number);
    BigInt(const std::string & decStr);
    BigInt(uint32_t * _dig,unsigned int _size) : digits(_dig), n(_size){}
    BigInt(BigInt && b);
    BigInt(const BigInt & b);

    BigInt operator+(const BigInt & b) const;
    BigInt operator-(const BigInt & b) const;
    BigInt operator*(const BigInt & b) const;
    BigInt operator/(const BigInt & b) const;

    BigInt &operator+=(const BigInt & b);
    BigInt &operator-=(const BigInt & b);
    BigInt &operator*=(const BigInt & b);
    BigInt &operator/=(const BigInt & b);


    BigInt operator<<(int shift) const;
    BigInt operator>>(int shift) const;

    bool operator<(const BigInt & b) const;
    bool operator>(const BigInt & b) const;
    bool operator==(const BigInt & b) const;
    bool operator!=(const BigInt & b) const;

    BigInt & operator=(const BigInt & b);
    BigInt & operator=(BigInt && b);
    //BigInt & operator()(const BigInt & b);

    //test functions (to be removed)
    std::string toHex() const;
    std::string toDec() const;
    std::string toBin() const;

    ~BigInt();
};

std::ostream & operator<<(std::ostream & stream,const BigInt & b);

template<typename... Args>
BigInt::BigInt(Args... digit)
{
    reallocate(sizeof...(digit));
    uint32_t tab[sizeof...(digit)] = {digit...};
    memcpy(digits,tab,sizeof(uint32_t)*sizeof...(digit));
}

#endif /*BIGINT*/