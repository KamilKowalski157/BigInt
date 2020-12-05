#include "BigInt.h"
#include <cstring>
#include <iostream>
uint32_t BigInt::endianMask = (uint32_t)1 << (sizeof(uint32_t) * 8 - 1);

template <typename T>
std::string var2Bin(const T &var)
{
    std::string result;
    char *ptr = (char *)&var;
    for (int i = 0; i < sizeof(T); i++)
    {
        for (int j = 0; j < 8; j++)
        {
            result += ((((ptr[sizeof(T) - 1 - i] << j) & 128) / 128) + '0');
            result += " ";
        }
    }
    return result;
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
    memset(ptr + n, sign * 255, sizeof(uint32_t) * (new_size - n));

    n = new_size;
    delete[] digits;
    digits = ptr;
}

std::string BigInt::toBin() const
{
    std::string result;
    for (int i = 0; i < n; i++)
    {
        result += var2Bin(digits[n - 1 - i]);
        result += "\t";
    }
    return result;
}

bool BigInt::abs()
{
    bool sign = isNegative();
    if (!sign)
    {
        return sign;
    }
    negate();
    return sign;
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
        return (n < b.n) * (!mySign);
    }
    for (int i = n - 1; i >= 0; --i)
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
        return (n > b.n) * (!mySign);
    }
    for (int i = n - 1; i >= 0; --i)
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
        if ((digits[i % n] * (i < n) + (!(i < n)) * ((~0) * mySign)) != (b.digits[i % n] * (i < b.n) + (!(i < b.n)) * ((~(uint32_t)0) * bSign))) // Skips over trailing zeros (also in case of negative numbers)
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
        if ((digits[i % n] * (i < n) + (!(i < n)) * ((~0) * mySign)) != (b.digits[i % n] * (i < b.n) + (!(i < b.n)) * ((~(uint32_t)0) * bSign))) // Skips over trailing zeros (also in case of negative numbers)
        {
            return true;
        }
    }
    return false;
}

//Arithmetic operators

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

BigInt BigInt::operator<<(int shift) const // Optimization is very much possible
{
    BigInt result(*this);
    buffer = 0;
    shift = -shift;

    int smallOffset = ((shift) % (sizeof(uint32_t) * 8));
    int bigOffset = shift - smallOffset;
    int biggerOffset = shift + (sizeof(uint32_t) * 8) - smallOffset;
    bool sign = isNegative();

    for (int i = 0; i < n; i++)
    {
        buffer = digits[(i + biggerOffset) % n] * ((i + biggerOffset) < n) * ((i + biggerOffset) >= 0) + ((i + biggerOffset) >= n) * (sign * (~0));
        buffer = (buffer << (sizeof(uint32_t) * 8));
        buffer += digits[(i + bigOffset) % n] * ((i + bigOffset) < n) * ((i + bigOffset) >= 0) + ((i + bigOffset) >= n) * (sign * (~0));

        result.digits[i] = (buffer >> (smallOffset));
    }
    return result;
}
BigInt BigInt::operator>>(int shift) const
{
    BigInt result(*this);
    buffer = 0;

    int smallOffset = ((shift) % (sizeof(uint32_t) * 8));
    int bigOffset = shift - smallOffset;
    int biggerOffset = shift + (sizeof(uint32_t) * 8) - smallOffset;
    bool sign = isNegative();

    for (int i = 0; i < n; i++)
    {
        buffer = digits[(i + biggerOffset) % n] * ((i + biggerOffset) < n) * ((i + biggerOffset) >= 0) + ((i + biggerOffset) >= n) * (sign * (~0));
        buffer = (buffer << (sizeof(uint32_t) * 8));
        buffer += digits[(i + bigOffset) % n] * ((i + bigOffset) < n) * ((i + bigOffset) >= 0) + ((i + bigOffset) >= n) * (sign * (~0));
        
        result.digits[i] = (buffer >> (smallOffset));
    }
    return result;
}

BigInt BigInt::operator/(const BigInt &b) const
{
    BigInt result;
    result.reallocate(std::max((int)n - (int)b.n + 1, 1));

    return result;
}
BigInt BigInt::operator*(const BigInt &b) const
{
}