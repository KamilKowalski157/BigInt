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

unsigned int BigInt::getActualSize() const
{
    for (int i = n - 1; i >= 0; --i)
    {
        if (digits[i] != 0)
        {
            int j;
            auto mask = endianMask;
            for (j = sizeof(uint32_t) * 8; j > 0; --j)
            {
                if (mask & digits[i])
                {
                    return i + j;
                }
                mask = (mask >> 1);
            }
        }
    }
    return -1;
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

void BigInt::shiftLeft()
{
    if (n == 1)
    {
        digits[0] = (digits[0] << 1);
        return;
    }
    uint64_t *ptr;
    for (int i = 0; i < n - 1; i++)
    {
        ptr = (uint64_t *)(digits + i);
        digits[i] = ((*ptr) << 1);
    }
    digits[n - 1] = ((*ptr) >> (sizeof(uint32_t) * 8));
}
void BigInt::shiftRight()
{
    if (n == 1)
    {
        digits[0] = (digits[0] >> 1);
        return;
    }
    uint64_t *ptr;
    for (int i = 0; i < n - 1; i++)
    {
        ptr = (uint64_t *)(digits + i);
        digits[i] = ((*ptr) >> 1);
    }
    digits[n - 1] = (((*ptr) >> (sizeof(uint32_t) * 8)) + endianMask * (!isNegative()));
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
    BigInt result;
    buffer = 0;
    shift = -shift;

    int smallOffset = ((shift) % (sizeof(uint32_t) * 8));
    int bigOffset = (shift - smallOffset) / (sizeof(uint32_t) * 8);
    int biggerOffset = (shift + (sizeof(uint32_t) * 8) - smallOffset) / (sizeof(uint32_t) * 8);

    result.reallocate(n - (bigOffset / (sizeof(uint32_t) * 8)));
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
    BigInt result;
    buffer = 0;

    int smallOffset = ((shift) % (sizeof(uint32_t) * 8));
    int bigOffset = (shift - smallOffset) / (sizeof(uint32_t) * 8);
    int biggerOffset = (shift + (sizeof(uint32_t) * 8) - smallOffset) / (sizeof(uint32_t) * 8);

    result.reallocate(n - (bigOffset / (sizeof(uint32_t) * 8)));
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
BigInt &BigInt::operator+=(const BigInt &b)
{
    if (b.n > n)
    {
        reallocate(b.n + 1);
    }
    bool mySign = isNegative();
    buffer = 0;
    for (int i = 0; i < std::max(b.n, n); i++)
    {
        buffer += digits[i];
        buffer += ((b.digits[i] * (i < b.n)) + ((!(i < b.n)) && (b.isNegative() * (~0)))); //prevents overflow
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    if (buffer == !mySign)
    {
        reallocate(n + 1);
        digits[n - 1] += buffer;
        digits[n - 1] += (~0) * b.isNegative();
    }
    return *this;
}
BigInt &BigInt::operator-=(const BigInt &b1)
{
    BigInt b(b1);
    if (b.n > n)
    {
        reallocate(b.n + 1);
    }
    b.negate();
    bool mySign = isNegative();
    buffer = 0;
    for (int i = 0; i < std::max(b.n, n); i++)
    {
        buffer += digits[i];
        buffer += ((b.digits[i] * (i < b.n)) + ((!(i < b.n)) && (b.isNegative() * (~0)))); //prevents overflow
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    if (buffer == !mySign)
    {
        reallocate(n + 1);
        digits[n - 1] += buffer;
        digits[n - 1] += (~0) * b.isNegative();
    }
    return *this;
}

BigInt BigInt::operator/(const BigInt &b) const
{
    if (b > *this)
    {
        return BigInt(0);
    }
    BigInt result;
    BigInt copy(*this);
    BigInt c = b;
    int diff = getActualSize()-b.getActualSize();
    c = (c<<diff);
    while(copy>0)
    {

    }

    return (result<<diff);
}
void BigInt::karatsuba(BigInt &a, BigInt &b)
{
}
BigInt BigInt::operator*(const BigInt &b) const
{
    if (n + b.n < 3)
    {
        BigInt simple_result;
        simple_result.reallocate(2);
        buffer = digits[0] * b.digits[0];
        simple_result.digits[0] = buffer;
        simple_result.digits[1] = (buffer << sizeof(uint32_t) * 8);
        return simple_result;
    }
    BigInt result;
    result.reallocate(n + b.n);
    //result.karatsuba();
    return result;
}