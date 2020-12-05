#include "BigInt.h"
#include <cstring>
#include <iostream>
uint32_t BigInt::endianMask = (uint32_t)1 << (sizeof(uint32_t) * 8 - 1);

template<typename T>
void var2Bin(const T & var)
{
    char * ptr = (char*)&var;
    for(int i = 0;i<sizeof(T);i++)
    {
        for(int j = 0;j<8;j++)
        {
            std::cout<<(int)(((ptr[sizeof(T)-1-i]<<j)&128)/128)<<" ";
        }
    }
    std::cout<<std::endl;
}

BigInt::BigInt(BigInt &&b)
{
    if (this != &b)
    {
        this->n = b.n;
        this->digits = b.digits;
        b.digits = nullptr;
    }
}
BigInt::BigInt(const BigInt &b)
{
    *this = b;
}

BigInt::~BigInt()
{
    delete[] digits;
}

// Utilities

BigInt &BigInt::operator=(BigInt &&b)
{
    std::swap(this->digits, b.digits);
    this->n = b.n;
    return *this;
}
BigInt &BigInt::operator=(const BigInt &b)
{
    this->digits = new uint32_t[b.n];
    this->n = b.n;
    memcpy(this->digits, b.digits, sizeof(uint32_t) * this->n);
    return *this;
}
std::ostream &operator<<(std::ostream &stream, const BigInt &b)
{
    BigInt buff(b);
    if (buff.isNegative())
    {
        stream << "-";
        buff.negate();
    }
    for (int i = 0; i < buff.n; i++)
    {
        stream << buff.digits[b.n - 1 - i] << "\t";
    }
    return stream;
}

void BigInt::negate()
{
    buffer = 1; //add 1 to least mattering bit
    for (int i = 0; i < n; i++)
    {
        buffer += (~digits[i]);
        digits[i] = buffer;
        buffer = buffer >> (sizeof(uint32_t) * 8);
    }
}
void BigInt::reallocate(unsigned int target_size)
{
    if (!(target_size > n))
    {
        return;
    }
    unsigned int new_size = n + (n == 0);
    while (new_size < target_size)
    {
        new_size = new_size << 1;
    }

    bool sign = isNegative();
    uint32_t *ptr = new uint32_t[new_size];

    memcpy(ptr, digits, n * sizeof(uint32_t));
    memset(ptr + n, sign*255, sizeof(uint32_t) * (new_size - n));

    n = new_size;
    delete[] digits;
    digits = ptr;
}

// Comparision operators

bool BigInt::operator<(const BigInt &b) const
{
    bool mySign = isNegative();
    bool bSign = b.isNegative();
    if (mySign ^ bSign)
    {
        return mySign;
    }
    if (n != b.n)
    {
        return (n < b.n);
    }
    for (int i = 0; i < n; i++)
    {
        if (digits[i] < b.digits[i])
        {
            return !mySign;
        }
    }
    return false;
}
bool BigInt::operator>(const BigInt &b) const
{
    bool mySign = isNegative();
    bool bSign = b.isNegative();
    if (mySign ^ bSign)
    {
        return bSign;
    }
    if (n != b.n)
    {
        return (n > b.n);
    }
    for (int i = 0; i < n; i++)
    {
        if (digits[i] > b.digits[i])
        {
            return !mySign;
        }
    }
    return false;
}
bool BigInt::operator==(const BigInt &b) const // fix to consider negative numbers
{
    bool mySign = isNegative();
    bool bSign = b.isNegative();
    for (unsigned int i = 0; i < std::max(n, b.n); i++)
    {
        if ((digits[i % n] * (i < n) + (!(i < n)) * ((~0) * mySign)) != (b.digits[i % n] * (i < b.n) + (!(i < b.n)) * ((~0) * bSign)))
        {
            return false;
        }
    }
    return true;
}
bool BigInt::operator!=(const BigInt &b) const
{
    bool mySign = isNegative();
    bool bSign = b.isNegative();
    
    for (unsigned int i = 0; i < std::max(n, b.n); i++)
    {
        if ((digits[i % n] * (i < n) + (!(i < n)) * ((~0) * mySign)) != (b.digits[i % n] * (i < b.n) + (!(i < b.n)) * ((~0) * bSign)))
        {
            return true;
        }
    }
    return false;
}
BigInt BigInt::operator+(const BigInt &b) const
{
    buffer = 0;
    unsigned int max_size = std::max(n + (digits[n - 1] != 0 & (!isNegative())) | (digits[n - 1] != (~0) & isNegative()),
                                     b.n + (b.digits[n - 1] != 0 & (!b.isNegative())) | (b.digits[n - 1] != (~0) & b.isNegative()));
    BigInt r1 = *this;
    BigInt r2 = b;
    r1.reallocate(max_size);
    r2.reallocate(max_size);
    for (int i = 0; i < r1.n; i++)
    {
        buffer += r1.digits[i];
        buffer += r2.digits[i];
        r1.digits[i] = buffer;
        buffer = (buffer >> (8 * sizeof(uint32_t)));
    }
    return r1;
}
BigInt BigInt::operator-(const BigInt &b) const
{
    buffer = 0;
    unsigned int max_size = std::max(n + (digits[n - 1] != 0 & (!isNegative())) | (digits[n - 1] != (~0) & isNegative()),
                                     b.n + (b.digits[n - 1] != 0 & (!b.isNegative())) | (b.digits[n - 1] != (~0) & b.isNegative()));
    BigInt r1 = *this;
    BigInt r2 = b;
    r2.negate();
    r1.reallocate(max_size);
    r2.reallocate(max_size);

    for (int i = 0; i < r1.n; i++)
    {
        buffer += r1.digits[i];
        buffer += r2.digits[i];
        r1.digits[i] = buffer;
        buffer = (buffer >> (8 * sizeof(uint32_t)));
    }
    return r1;
}