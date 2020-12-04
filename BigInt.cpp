#include "BigInt.h"
#include <cstring>

uint32_t BigInt::endianMask = (uint32_t)1 << (sizeof(uint32_t) * 8 - 1);

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
    for (int i = 0; i < b.n; i++)
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
        buffer += (!digits[i]);
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
    uint32_t *ptr = new uint32_t[new_size];
    memcpy(ptr, digits, n * sizeof(uint32_t));
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
bool BigInt::operator==(const BigInt &b) const
{
    for (int i = 0; i < std::min(n, b.n); i++)
    {
        if (digits[i] != b.digits[i])
        {
            return false;
        }
    }
    return true;
}
bool BigInt::operator!=(const BigInt &b) const
{
    for (int i = 0; i < std::min(n, b.n); i++)
    {
        if (digits[i] != b.digits[i])
        {
            return true;
        }
    }
    return false;
}
BigInt BigInt::operator+(const BigInt &b) const
{
    buffer = 0;
    BigInt result;
    result.reallocate(std::max(n + (digits[n - 1] != 0), b.n + (b.digits[b.n - 1] != 0)));

    unsigned int i;
    for (i = 0; i < std::max(n, b.n); i++)
    {
        buffer += digits[i];
        buffer += b.digits[i];
        result.digits[i] = buffer;
        buffer = (buffer << (sizeof(uint32_t) * 8));
    }
    if (n < b.n)
    {
        for (; i < b.n; i++)
        {
            buffer += b.digits[i];
            result.digits[i] = buffer;
            buffer = (buffer << (sizeof(uint32_t) * 8));
        }
        return result;
    }
    for (; i < n; i++)
    {
        buffer += digits[i];
        result.digits[i] = buffer;
        buffer = (buffer << (sizeof(uint32_t) * 8));
    }
    return result;
}