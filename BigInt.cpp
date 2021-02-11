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
        }
    }
    return result;
}

BigInt::BigInt(const std::string &str) : owner(true)
{
    int size = (str.size() / 8) + 1;
    reallocate(size);
    uint32_t d_buff;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0, k = i * 8; j < 8; j++, k++)
        {
            digits[i] += (((str[(str.size() - 1 - k) % str.size()] - '0') * ((str.size() - 1 - k) < str.size())) << (j * 4));
        }
    }
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            d_buff = (d_buff >> 1);
            d_buff += (1 << (sizeof(uint32_t) * 8 - 1)) * (digits[i] & 1);

            buffer = 0;
            for (int k = n; k >= i; k--)
            {
                buffer += digits[k];
                digits[k] = (buffer >> 1);
                buffer = (buffer << (sizeof(uint32_t) * 8));
            }

            for (int k = i; k < n; k++)
            {
                for (int l = 0, base = 1; l < 8; l++, base = (base << 4))
                {
                    if (digits[k] & (base << 3))
                    {
                        digits[k] -= base * 3;
                    }
                }
            }
        }
        for (int j = n - 1; j > i; j--)
        {
            digits[j] = digits[j - 1];
        }
        digits[i] = d_buff;
    }
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

BigInt::BigInt(BigInt &&b) : owner(b.owner)
{
    if (this != &b)
    {
        deallocate();
        this->n = b.n;
        this->digits = b.digits;
        sign = b.sign;
        b.digits = nullptr;
    }
}
BigInt::BigInt(const BigInt &b) : owner(b.owner)
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
    if (b.n < n)
    {
        (*this) = (BigInt &)b;
        return *this;
    }
    deallocate();
    std::swap(this->digits, b.digits);
    this->n = b.n;
    sign = b.sign;
    return *this;
}
BigInt &BigInt::operator=(const BigInt &b)
{
    //owner = (reallocate(b.n) | owner);
    reallocate(b.n);
    memset(this->digits, (~0) * b.sign, n * sizeof(uint32_t));
    memcpy(this->digits, b.digits, sizeof(uint32_t) * std::min(b.n, n));
    sign = b.sign;
    return *this;
}
std::string BigInt::toDec() const
{
    std::string res;
    int size = ((n * 1.204) + 1); //ceiling, 1.204 = log_10(16)
    uint32_t *tab = new uint32_t[size];
    memset(tab, 0, size * sizeof(uint32_t));
    buffer = 0;

    for (int i = n - 1; i >= 0; --i)
    {
        for (int j = sizeof(uint32_t) * 8 - 1; j >= 0; --j)
        {
            for (int k = 0; k < size; ++k)
            {
                for (int l = 0, base = 1; l < 8; ++l, base = (base << 4))
                {
                    if (((tab[k] >> (l * 4)) % 16) > 4)
                    {
                        tab[k] += (3 * base);
                    }
                }
            }
            buffer = 0;
            for (int k = 0; k < size; ++k)
            {
                buffer += (((uint64_t)tab[k]) * 2);
                tab[k] = buffer;
                buffer = (buffer >> (sizeof(uint32_t) * 8));
            }
            if (sign)
            {
                tab[0] += (((~(*this)[i]) & (1 << (j))) != 0);
                continue;
            }
            tab[0] += (((*this)[i] & (1 << (j))) != 0);
        }
    }
    int i, j;
    for (i = size - 1; i >= 0; --i)
    {
        for (j = 7; j >= 0; --j)
        {
            if (((tab[i] >> (j * 4)) % 16) != 0)
            {
                break;
            }
        }
        if (((tab[i] >> (j * 4)) % 16) != 0)
        {
            break;
        }
    }
    if (sign)
    {
        res += "-";
    }
    char digit = sign;
    char carry = 0;
    for (; i >= 0; --i, j = 7)
    {
        for (; j >= 0; --j)
        {
            digit += ((tab[i] >> (j * 4)) % 16);
            carry = digit / 10;
            res += ('0' + digit);
            digit = carry;
        }
    }
    delete[] tab;
    return res;
}
std::ostream &operator<<(std::ostream &stream, const BigInt &_b)
{
    stream << _b.toDec();
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
        if ((*this)[i] == b[i])
        {
            continue;
        }
        return (!mySign) ^ ((*this)[i] < b[i]);
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
        if ((*this)[i] == b[i])
        {
            continue;
        }
        return (!mySign) ^ ((*this)[i] > b[i]);
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

BigInt BigInt::operator<<(int shift) const
{
    if (shift <= 0)
    {
        if (shift == 0)
        {
            return *this;
        }
        return ((*this) >> (-shift));
    }
    int majorOffset = shift / (sizeof(uint32_t) * 8);
    int minorOffset = (shift - majorOffset * (sizeof(uint32_t) * 8));
    for (int i = n - 1; i >= 0; --i)
    {
        buffer = ((*this)[i - majorOffset - 1] >> (sizeof(uint32_t) * 8 - minorOffset)) * (minorOffset != 0);
        buffer += ((*this)[i - majorOffset] << (minorOffset));
        digits[i] = buffer;
    }
    return (*this);
}
BigInt BigInt::operator>>(int shift) const
{
    if (shift <= 0)
    {
        if (shift == 0)
        {
            return *this;
        }
        return ((*this) << (-shift));
    }
    int majorOffset = shift / (sizeof(uint32_t) * 8);
    int minorOffset = (shift - majorOffset * (sizeof(uint32_t) * 8));
    for (int i = 0; i < n; ++i)
    {
        buffer = ((*this)[i + majorOffset] >> (minorOffset));
        buffer += ((*this)[i + 1 + majorOffset] << (sizeof(uint32_t) * 8 - minorOffset)) * (minorOffset != 0);
        digits[i] = buffer;
    }
    return (*this);
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
    if(b==0){throw std::exception();}
    BigInt a;
    a.reallocate(2*n);
    a = *this;
    return (((a)*b.computeInverse(n))>>(n*32))+BigInt(1);
}
BigInt BigInt::operator%(const BigInt &b) const
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
    divisor = (divisor << (s1 - s2));
    BigInt one[2]{1, -1};
    while (s1 >= s2)
    {
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
    if (buffer.sign)
    {
        buffer += b;
    }
    return buffer;
}

void BigInt::mulAdd(const BigInt &a, uint64_t b)
{
    buffer = 0;
    a.buffer = b;
    for (int i = 0; i < n; i++)
    {
        buffer += (a.buffer * a[i]);
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
}
void BigInt::karatsuba(const BigInt &a, const BigInt &b, BigInt &buff) // Should not modify its parameters
{
    if ((a.n != 0) * (b.n != 0) * (n != 0) == 0)
    {
        this->clear();
        return;
    }
    if ((b.n - 1) * (a.n - 1) == 0)
    {
        if (b.n == 1)
        {
            mulAdd(a, b.digits[0]);
            return;
        }
        mulAdd(b, a.digits[0]);
        return;
    }

    unsigned int mid = std::max(a.n + 1, b.n + 1) / 2;
    // I have guarantee that a1.n>=a2.n and b1.n>=b2.n

    BigInt a1(a, 0, mid);
    BigInt a2(a, a1.n, mid);
    BigInt b1(b, 0, mid);
    BigInt b2(b, b1.n, mid);

    BigInt r1((*this), 0, 2 * mid);
    BigInt r2((*this), mid, n - mid);
    BigInt r3((*this), 2 * mid, n - 2 * mid);

    BigInt buf1(buff, 0, 2 * mid);
    BigInt buf4(buff, buf1.n, buff.n - buf1.n);

    karatsuba(a1, b1, buf1);
    r3.karatsuba(a2, b2, buf1);

    bool sign_a = false;
    bool sign_b = true;
    a1 -= a2;
    if (a1.sign)
    {
        sign_a = true;
        a1.negate();
    }
    b1 -= b2;
    if (b1.sign)
    {
        sign_b = false;
        b1.negate();
    }

    buff = r1;
    buff += r3;
    r2 += buff;

    buf1.karatsuba(a1, b1, buf4);

    if (sign_a)
    {
        a1.negate();
    }
    if (!sign_b)
    {
        b1.negate();
    }
    b1 += b2;
    a1 += a2;
    if (sign_a ^ sign_b)
    {
        r2 -= buf1;
        return;
    }
    r2 += buf1;
}
BigInt BigInt::operator*(const BigInt &_b) const
{
    BigInt result, buff1;
    result.reallocate(n);
    buff1.reallocate(n + _b.n);
    result.karatsuba(*this, _b, buff1);
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
//TODO: Use karatsuba directly
BigInt BigInt::computeInverse(unsigned int k) const // Newton-Raphson
{
    buffer = 1;
    buffer = (buffer<<(sizeof(uint64_t)*8-1));//good idea- continue
    BigInt a(1);
    BigInt kbuff;
    BigInt buff;
    BigInt res;
    kbuff.reallocate(2*k);
    buff.reallocate(k+n+1);
    res.reallocate(2*k+1);
    a.reallocate(k+1);
    k *= 32;
    a = (a << (k - n * 32));
    for (int i = 0; i < k; ++i)
    {
        buff = (*this);
        buff.karatsuba(buff,a,kbuff);
        res = 2;
        res = (res << k);
        res -= buff;
        res.karatsuba(res,a,kbuff);
        res = (res >> k);
        if (res == a)
        {
            break;
        }
        a = res;
    }
    return a;
}