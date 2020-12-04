#ifndef BIGINT
#define BIGINT
#include <stdint.h>
#include <string>

class BigInt
{
    static uint64_t endianMask;
    uint64_t *digits;
    unsigned int n;//length of digits table

    void reallocate(unsigned int target_size);
public:
    template<typename T>
    BigInt(const T & number){reallocate(1);digits[0] = number;}
    BigInt(const std::string & decStr);
    BigInt(uint64_t * _dig,unsigned int _size) : digits(_dig), n(_size){}

    BigInt && operator+(const BigInt & b);
    BigInt && operator-(const BigInt & b);
    BigInt && operator*(const BigInt & b);
    BigInt && operator/(const BigInt & b);

    bool operator<(const BigInt & b);
    bool operator>(const BigInt & b);
    bool operator==(const BigInt & b);

    BigInt & operator=(const BigInt & b);
    BigInt & operator()(const BigInt & b);
};

#endif /*BIGINT*/