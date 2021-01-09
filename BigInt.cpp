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
        sign = nullptr;
        n = 0;
    }
}

BigInt::BigInt(BigInt &&b)
{
    if ((this != &b) && b.owner)
    {
        deallocate();
        this->n = b.n;
        this->digits = b.digits;
        sign = b.sign;
        b.digits = nullptr;
        b.sign = nullptr;
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
    if (owner ^ b.owner) // ownership types have to match
    {
        reallocate(b.n);
        memset(this->digits, (~0) * (*b.sign), n * sizeof(uint32_t));
        memcpy(this->digits, b.digits, sizeof(uint32_t) * std::min(b.n, n));
        *sign = *b.sign;
        return *this;
    }

    deallocate();
    std::swap(this->digits, b.digits);
    this->n = b.n;
    sign = b.sign;
    b.sign = nullptr;
    return *this;
}
BigInt &BigInt::operator=(const BigInt &b)
{
    reallocate(b.n);
    memset(this->digits, (~0) * (*b.sign), n * sizeof(uint32_t));
    memcpy(this->digits, b.digits, sizeof(uint32_t) * std::min(b.n, n));
    *sign = *b.sign;
    return *this;
}
std::ostream &operator<<(std::ostream &stream, const BigInt &b)
{
    stream << b.isNegative() << " . ";
    int64_t buff = b.isNegative();
    for (int i = b.n - 1; i >= 0; i--)
    {
        buff += ((!b.isNegative()) * b.digits[i]) + ((~b.digits[i]) * b.isNegative());
        stream << ((uint32_t)buff) << "\t";
        buff = (buff >> (sizeof(uint32_t) * 8));
    }
    return stream;
}

void BigInt::negate()
{
    //uint32_t mask =0;
    buffer = 1; //add 1 to least mattering bit
    for (int i = 0; i < n; i++)
    {
        //mask = (mask|digits[i]);
        buffer += (~digits[i]);
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    *sign += (1+buffer);
    //*sign *= (mask!=1);
}
bool BigInt::reallocate(unsigned int target_size)
{
    if ((!(target_size > n)) || !owner)
    {
        return false;
    }
    unsigned int new_size = n + (n == 0);
    while (new_size < target_size)
    {
        new_size = new_size << 1;
    }

    uint32_t *ptr = new uint32_t[new_size + 1];

    memcpy(ptr, digits, n * sizeof(uint32_t));
    memset(ptr + n, ((*sign) & 1) * 255, sizeof(uint32_t) * (new_size - n));
    ptr[new_size] = *sign;

    deallocate();
    n = new_size;
    digits = ptr;
    sign = (digits + n);
    return true;
}

void BigInt::clear()
{
    *sign = 0;
    for (int i = 0; i < n; i++)
    {
        digits[i] = 0;
    }
}

std::string BigInt::toBin() const
{
    std::string result;
    result += '0'+(char)isNegative();
    result+=".";
    for (int i = 0; i < n; i++)
    {
        result += var2Bin(digits[n - 1 - i]);
        //result += "\t";
    }
    return result;
}

bool BigInt::abs()
{
    if (isNegative())
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
        if (digits[i] != (~0) * isNegative())
        {
            int j;
            auto mask = endianMask;
            for (j = sizeof(uint32_t) * 8; j > 0; --j)
            {
                if ((mask & digits[i]) ^ isNegative())
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
    if (isNegative() ^ b.isNegative())
    {
        return isNegative();
    }
    for (int i = std::max(n - 1, b.n - 1); i >= 0; --i)
    {
        if ((*this)[i] < b[i])
        {
            return !isNegative();
        }
    }
    return false;
}
bool BigInt::operator>(const BigInt &b) const
{
    if (isNegative() ^ b.isNegative())
    {
        return b.isNegative();
    }
    for (int i = n - 1; i >= 0; --i)
    {
        if ((*this)[i] > b[i])
        {
            return !isNegative();
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
    digits[n - 1] = (((*ptr) >> (sizeof(uint32_t) * 8) + 1) + endianMask * isNegative());
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
        buffer += b[i];
        r1.digits[i] = buffer;
        buffer = (buffer >> (8 * sizeof(uint32_t)));
    }
    *r1.sign = ((*r1.sign + *b.sign + (buffer != 0)) & 1);
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
        buffer += r1[i];
        buffer += digits[i % n] * (i < n);
        r1.digits[i] = buffer;
        buffer = (buffer >> (8 * sizeof(uint32_t)));
    }
    *r1.sign = ((*sign - *r1.sign + (buffer != 0)) & 1);
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
BigInt &BigInt::operator+=(const BigInt &b) // FIXME
{
    buffer = 0;
    unsigned int min = std::min(n, b.n);
    unsigned int i;
    for (i = 0; (i < n) && (buffer || i < b.n); ++i)
    {
        buffer += digits[i];
        buffer += b[i];
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    *sign += (*b.sign + buffer);
    return *this;
}
BigInt &BigInt::operator-=(const BigInt &b)
{
    buffer = (uint32_t)(1);
    unsigned int min = std::min(n, b.n);
    unsigned int i;
    for (i = 0; (i < n) && (buffer || i < b.n); ++i)
    {
        buffer += digits[i];
        buffer += (~b[i]);
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    *sign += (buffer + (!*b.sign));
    return *this;
}

BigInt BigInt::computeInverse() const
{
    BigInt result(-1);
    BigInt buffer;
    int k = 10;
    std::cout << "for: " << (*this) << std::endl;
    std::cout << "and: " << result << std::endl;
    while (true)
    {
        buffer = result * (*this);
        buffer = (buffer >> k);
        buffer.negate();
        result = result * (buffer + 2);
        std::cout << result << " " << (((*this) * result) >> k) << std::endl;
        std::cin.get();
    }
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
        result += one[buffer.isNegative()];
        if (buffer.isNegative())
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
    result -= one[buffer.isNegative()] * (buffer.isNegative());
    if (sign)
    {
        result.negate();
    }
    return result;
}

void BigInt::addMul(const BigInt &_a, const BigInt &_b)
{
    buffer = 0;
    const BigInt *a = &_a;
    const BigInt *b = &_b;
    if(_a.n==1)
    {
        a = &_b;
        b = &_a;
    }
    uint64_t compl1 = ((~(uint64_t)0)<<(sizeof(uint32_t)*8))*a->isNegative();
    uint64_t compl2 = ((~(uint64_t)0)<<(sizeof(uint32_t)*8))*b->isNegative();
    uint64_t compl3 = ((~(uint64_t)0)<<(sizeof(uint32_t)*8))*(b->isNegative()^a->isNegative());
    b->buffer = compl2+b->digits[0];
    unsigned int i;
    for (i = 0; i < a->n; ++i)
    {
        a->buffer = a->digits[i];
        a->buffer += compl1;
        a->buffer *= b->buffer;
        buffer += a->buffer;
        buffer += digits[i];
        digits[i] = buffer;
        buffer = (buffer>>(sizeof(uint32_t)*8));
    }
    for(;(i<n)&&buffer;++i)
    {
        buffer += compl3;
        buffer += digits[i];
        digits[i] = buffer;
        buffer = (buffer>>(sizeof(uint32_t)*8));
    }
}

uint64_t BigInt::carryAdd(const BigInt &a, uint64_t b)
{
    buffer = b;
    unsigned int i;
    for (i = 0; i < std::min(n, a.n); ++i)
    {
        buffer += digits[i];
        buffer += a.digits[i];
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    for (; (i < n)&&buffer; ++i)
    {
        buffer += digits[i];
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    return buffer;
}

void BigInt::karatsuba(BigInt &_a, BigInt &_b, BigInt &result, BigInt &buff)
{
    if (result.n * _a.n * _b.n == 0)
    {
        return;
    }
    if ((_a.n - 1) * (_b.n - 1) == 0)
    {
        result.addMul(_a, _b);
        return;
    }

    BigInt &a = (_a.n < _b.n) ? _b : _a; // greater
    BigInt &b = (_a.n < _b.n) ? _a : _b; // lesser

    unsigned int mid = a.n / 2;

    BigInt a1(a, 0, mid);
    BigInt a2(a, a1.n, a.n - a1.n);
    BigInt b1(b, 0, std::min(mid, b.n));
    BigInt b2(b, b1.n, b.n - b1.n);

    BigInt r1(result, 0, result.n);
    BigInt r11(result, 0, mid);
    BigInt r12(result, mid, mid);

    BigInt r2(result, 2 * mid, result.n - (2 * mid));
    BigInt r21(result, 2 * mid, mid);
    BigInt r22(result, 3 * mid, mid);

    karatsuba(a1, b1, r1, buff);
    karatsuba(a2, b2, r2, buff);

    uint64_t buff1 = r12.carryAdd(r21, 0);
    r21 = r12;
    uint64_t buff2 = r21.carryAdd(r22, buff1);
    r22 += buff2;
    r22 += buff1;
    uint64_t buff3 = r12.carryAdd(r11, 0);
    r2 += buff3;

    BigInt r3(result,  mid, result.n -  mid);
    a1 -= a2;
    b1.negate();
    b1 += b2;

    karatsuba(a1, b1, r3, buff);

    b1 -= b2;
    b1.negate();
    a1 += a2;
}
BigInt BigInt::operator*(const BigInt &_b) const
{
    BigInt result, a, b, buff1;
    a = *this;
    b = _b;
    result.reallocate(n);
    buff1.reallocate(result.n);
    a.n /= 2;
    b.n /= 2;
    bool sign = a.abs() ^ b.abs();
    result.karatsuba(a, b, result, buff1);
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
    if (bSign ^ isNegative())
    {
        result.negate();
    }
    return result;
}
BigInt &BigInt::operator*=(uint32_t b)
{
    buffer = 0;
    for (int i = 0; i < n; i++)
    {
        buffer += (uint64_t)digits[i] * (uint64_t)b;
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    return *this;
}
BigInt &BigInt::operator*=(const BigInt &_b)
{
    BigInt a, b, buff1;
    a = *this;
    b = _b;
    this->reallocate(n + b.n);
    buff1.reallocate(this->n);
    bool sign = a.abs() ^ b.abs();
    this->karatsuba(a, b, *this, buff1);
    if (sign)
    {
        negate();
    }
    return *this;
}