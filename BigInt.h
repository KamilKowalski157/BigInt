#ifndef BIGINT
#define BIGINT

#include <stdint.h>
#include <string>
#include <ostream>

class BigInt
{
    static uint32_t endianMask;
    uint32_t *digits = nullptr;
    mutable uint64_t buffer;
    unsigned int n = 0;//length of digits table

    void reallocate(unsigned int target_size);
    bool isNegative() const {return (digits!=nullptr&&(digits[n]&endianMask));}
    void negate();

    friend std::ostream & operator<<(std::ostream & stream,const BigInt & b);
public:
    BigInt(){}
    template<typename T>
    BigInt(const T & number);
    BigInt(const std::string & decStr);
    BigInt(uint32_t * _dig,unsigned int _size) : digits(_dig), n(_size){}
    BigInt(BigInt && b);
    BigInt(const BigInt & b);

    BigInt operator+(const BigInt & b) const;
    BigInt operator-(const BigInt & b) const;
    BigInt operator*(const BigInt & b) const;
    BigInt operator/(const BigInt & b) const;

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

template<typename T>
BigInt::BigInt(const T &digit)
{
    reallocate(1);
    digits[0]  = digit*(1-2*(digit<0));
    if(digit<0)
    {
        negate();
    }
}

#endif /*BIGINT*/