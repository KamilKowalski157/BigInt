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
            //result += " ";
        }
    }
    return result;
}

void BigInt::deallocate()
{
    if (owner)
    {
        if (digits != nullptr)
        {
            delete[] digits;
        }
        digits = nullptr;
        n = 0;
        sign = 0;
    }
}

BigInt::BigInt(BigInt &&b)
{
    if (this != &b)
    {
        deallocate();
        this->n = b.n;
        this->digits = b.digits;
        sign = b.sign;
        owner = b.owner;
        b.digits = nullptr;
    }
}
BigInt::BigInt(const BigInt &b)
{
    *this = b;
}

BigInt::~BigInt()
{
    deallocate();
}

// Utilities

BigInt &BigInt::operator=(BigInt &&b)
{
    deallocate();
    std::swap(this->digits, b.digits);
    this->n = b.n;
    sign = b.sign;
    owner = b.owner;
    return *this;
}
BigInt &BigInt::operator=(const BigInt &b)
{
    owner = (reallocate(b.n) | owner);
    memset(this->digits, (~0) * b.sign, n * sizeof(uint32_t));
    memcpy(this->digits, b.digits, sizeof(uint32_t) * b.n);
    sign = b.sign;
    return *this;
}
std::ostream &operator<<(std::ostream &stream, const BigInt &b)
{
    BigInt buff(b);
    stream << buff.sign << " . ";
    if (buff.sign)
    {
        //buff.negate();
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
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    sign = ((sign == 0) + (buffer != 0)) & 1;
}
bool BigInt::reallocate(unsigned int target_size)
{
    if (!(target_size > n) || !owner)
    {
        return false;
    }
    unsigned int new_size = n + (n == 0);
    while (new_size < target_size)
    {
        new_size = new_size << 1;
    }

    uint32_t *ptr = new uint32_t[new_size];

    memcpy(ptr, digits, n * sizeof(uint32_t));
    memset(ptr + n, sign * 255, sizeof(uint32_t) * (new_size - n));

    deallocate();
    n = new_size;
    digits = ptr;
    return true;
}

void BigInt::clear()
{
    sign = false;
    for (int i = 0; i < n; i++)
    {
        digits[i] = 0;
    }
}

std::string BigInt::toBin() const
{
    std::string result;
    for (int i = 0; i < n; i++)
    {
        result += var2Bin(digits[n - 1 - i]);
        //result += "\t";
    }
    return result;
}

bool BigInt::abs()
{
    if (sign)
    {
        negate();
        return true;
    }
    return false;
}

unsigned int BigInt::getActualSize() const
{
    for (int i = n - 1; i >= 0; --i)
    {
        if (digits[i] != (~0) * sign)
        {
            int j;
            auto mask = endianMask;
            for (j = sizeof(uint32_t) * 8; j > 0; --j)
            {
                if ((mask & digits[i]) ^ sign)
                {
                    return i * sizeof(uint32_t) * 8 + j;
                }
                mask = (mask >> 1);
            }
        }
    }
    return -1;
}

// Comparision operators are invalid

bool BigInt::operator<(const BigInt &b) const
{
    bool mySign = sign;
    bool bSign = b.sign;
    if (mySign ^ bSign)
    {
        return mySign;
    }
    for (int i = std::max(n - 1, b.n - 1); i >= 0; --i)
    {
        if ((*this)[i] < b[i])
        {
            return !mySign;
        }
    }
    return false;
}
bool BigInt::operator>(const BigInt &b) const
{
    bool mySign = sign;
    bool bSign = b.sign;
    if (mySign ^ bSign)
    {
        return bSign;
    }
    for (int i = n - 1; i >= 0; --i)
    {
        if ((*this)[i] > b[i])
        {
            return !mySign;
        }
    }
    return false;
}
bool BigInt::operator==(const BigInt &b) const // fix to consider negative numbers
{
    for (unsigned int i = 0; i < std::max(n, b.n); i++)
    {
        if ((*this)[i] != b[i])
        {
            return false;
        }
    }
    return true;
}
bool BigInt::operator!=(const BigInt &b) const
{
    for (unsigned int i = 0; i < std::max(n, b.n); i++)
    {
        if ((*this)[i] != b[i])
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
    for (int i = n - 1; i > 0; i--)
    {
        ptr = (uint64_t *)(digits + i - 1);
        digits[i] = ((*ptr) >> (sizeof(uint32_t) * 8 - 1));
    }
    digits[0] = (digits[0] << 1);
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
    digits[n - 1] = (((*ptr) >> (sizeof(uint32_t) * 8) + 1) + endianMask * sign);
}

//Arithmetic operators

BigInt BigInt::operator+(const BigInt &b) const
{
    buffer = 0;
    BigInt r1;
    r1.reallocate(std::max(n, b.n));
    for (int i = 0; i < r1.n; i++) // Overflows as intended...
    {
        buffer += digits[i % n] * (i < n);
        buffer += b.digits[i % b.n] * (i < b.n);
        r1.digits[i] = buffer;
        buffer = (buffer >> (8 * sizeof(uint32_t)));
    }
    r1.sign = ((r1.sign + b.sign + (buffer != 0)) & 1);
    return r1;
}

BigInt BigInt::operator-(const BigInt &b) const
{
    buffer = 0;
    BigInt r1 = b;
    r1.negate();
    r1.reallocate(std::max(n, b.n));

    for (int i = 0; i < r1.n; i++)
    {
        buffer += r1.digits[i];
        buffer += digits[i % n] * (i < n);
        r1.digits[i] = buffer;
        buffer = (buffer >> (8 * sizeof(uint32_t)));
    }
    r1.sign = ((sign - r1.sign + (buffer != 0)) & 1);
    return r1;
}

BigInt BigInt::operator<<(int shift) const // Optimization is very much possible
{
    if (shift == 0)
    {
        return *this;
    }
    shift = -shift;
    BigInt result;
    result.reallocate(n);
    int majorOffset = (shift * (1 - (shift < 0) * 2) / (sizeof(uint32_t) * 8)) * (1 - (shift < 0) * 2) - (shift < 0);
    int minorOffset = shift - majorOffset * sizeof(uint32_t) * 8;
    uint64_t buf;
    for (int i = 0, j = majorOffset; i < result.n; i++, j++)
    {
        buf = (*this)[j + 1];
        buf = (buf << (sizeof(uint32_t) * 8));
        buf += (*this)[j];
        result.digits[i] = (buf >> (minorOffset));
    }
    return result;
}
BigInt BigInt::operator>>(int shift) const
{
    if (shift == 0)
    {
        return *this;
    }
    BigInt result;
    result.reallocate(n);
    int majorOffset = (shift * (1 - (shift < 0) * 2) / (sizeof(uint32_t) * 8)) * (1 - (shift < 0) * 2) - (shift < 0);
    int minorOffset = shift - majorOffset * sizeof(uint32_t) * 8;
    uint64_t buf;
    for (int i = 0, j = majorOffset; i < result.n; i++, j++)
    {
        buf = (*this)[j + 1];
        buf = (buf << (sizeof(uint32_t) * 8));
        buf += (*this)[j];
        result.digits[i] = (buf << (minorOffset));
    }
    return result;
}
BigInt &BigInt::operator+=(const BigInt &b)
{
    buffer = 0;
    for (int i = 0; i < n; i++)
    {
        buffer += digits[i];
        buffer += b[i];
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    sign = ((sign + b.sign + (buffer != 0)) & 1);
    return *this;
}
BigInt &BigInt::operator-=(const BigInt &b)
{
    buffer = (uint32_t)(1);
    for (int i = 0; i < n; i++)
    {
        buffer += digits[i];
        buffer += (~b[i]);
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    sign = ((sign + (!b.sign) + (buffer != 0)) & 1);
    return *this;
}

BigInt BigInt::operator/(const BigInt &b) const
{
    if (b == 0)
    {
        throw std::exception();
    }
    BigInt buffer = *this;
    BigInt divisor = b;
    divisor.reallocate(n);
    bool sign = divisor.abs() ^ buffer.abs();
    int s1 = buffer.getActualSize();
    int s2 = divisor.getActualSize();
    BigInt result;
    result.reallocate(std::max(buffer.n, divisor.n));
    divisor = (divisor << (s1 - s2));
    BigInt one[2]{1, -1};
    while (s1 >= s2)
    {
        result.shiftLeft();
        result += one[buffer.sign];
        if (buffer.sign)
        {
            buffer += divisor;
        }
        else
        {
            buffer -= divisor;
        }
        divisor.shiftRight();
        s1--;
    }
    result -= one[buffer.sign] * buffer.sign;
    return result;
}
void BigInt::karatsuba(BigInt &a, BigInt &b, BigInt &buff)
{
    if (a.n * b.n == 0)
    {
        return;
    }
    if ((b.n - 1) * (a.n - 1) == 0)
    {
        if (b.n == 1)
        {
            (*this) = a;
            (*this) *= b.digits[0];
            return;
        }
        (*this) = b;
        (*this) *= a.digits[0];
        return;
    }

    unsigned int mid = std::max(a.n, b.n) / 2;

    BigInt a1(a, 0, mid);
    BigInt a2(a, a1.n, a.n - a1.n);
    BigInt b1(b, 0, mid);
    BigInt b2(b, b1.n, b.n - b1.n);

    BigInt r1((*this), 0, 2 * mid);
    BigInt r2((*this), mid, n - mid);
    BigInt r3((*this), +2 * mid, 2 * mid);

    BigInt buf1(buff, 0, 2 * mid);
    //BigInt buf2(buff, buf1.n, mid);
    //BigInt buf3(buff, buf1.n + buf2.n, buff.n - buf1.n - buf2.n);
    BigInt buf4(buff, buf1.n, buff.n - buf1.n);

    //buf2 = a1;
    //buf2 -= a2;
    //buf3 = b2;
    //buf3 -= b1;

    r1.karatsuba(a1, b1, buf1);
    r3.karatsuba(a2, b2, buf1);

    a1 -= a2;
    b1.negate();
    b1 += b2;

    //a1 = buf2;
    //a2 = buf3;

    bool signa = a1.abs();
    bool signb = b1.abs();

    buff = r1;
    buff += r3;
    r2 += buff;

    buf1.karatsuba(a1, b1, buf4);

    if (signa)
    {
        a1.negate();
    }
    if (signb)
    {
        b1.negate();
    }

    b1 -= b2;
    b1.negate();
    a1 += a2;

    if (signa ^ signb)
    {
        buf1.negate();
    }
    r2 += buf1;
}
BigInt BigInt::operator*(const BigInt &_b) const
{
    BigInt result, a, b, buff1;
    a = *this;
    b = _b;
    result.reallocate(n + b.n);
    buff1.reallocate(result.n);
    bool sign = a.abs() ^ b.abs();
    result.karatsuba(a, b, buff1);
    if (sign)
    {
        result.negate();
    }
    return result;
}
BigInt BigInt::operator*(int32_t b) const
{
    bool bSign = (b < 0);
    b = b * (1 - (2 * bSign));
    BigInt result = *this;
    buffer = 0;
    for (int i = 0; i < n; i++)
    {
        buffer += (uint64_t)result.digits[i] * (uint64_t)b;
        result.digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    if (bSign ^ sign)
    {
        result.negate();
    }
    return result;
}
BigInt &BigInt::operator*=(uint32_t b)
{
    buffer = 0;
    for (int i = 0; i < n; ++i)
    {
        buffer += (uint64_t)digits[i] * (uint64_t)b;
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    return *this;
}