#include "BigInt.h"
#include <cstring>
#include <iostream>

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

BigInt::BigInt(const std::string &str) : size(allocate((str.size() / 8) + 1))
{
    bool negative = false;
    if (str[0] == '-')
    {
        negative = true; // TODO
    }
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
            for (int k = size-1; k >= i; k--)
            {
                buffer += digits[k];
                digits[k] = (buffer >> 1);
                buffer = (buffer << (sizeof(uint32_t) * 8));
            }

            for (int k = i; k < size; k++)
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
        for (int j = size - 1; j > i; j--)
        {
            digits[j] = digits[j - 1];
        }
        digits[i] = d_buff;
    }
}

void BigInt::deallocate()
{
    if (digits != nullptr)
    {
        delete[] digits;
    }
    digits = nullptr;
}

BigInt::BigInt(BigInt &&b) : size(b.size), digits(b.digits), sign(b.sign)
{
    b.digits = nullptr;
}
BigInt::BigInt(const BigInt &b) : size(b.size), sign(b.sign)
{
    if (b.digits != nullptr)
    {
        allocate(size);
        memcpy(digits, b.digits, sizeof(uint32_t) * size);
    }
    else{}
}

BigInt::~BigInt()
{
    deallocate();
}

BigInt &BigInt::operator=(BigInt &&b)
{
    if (b.size < size)
    {
        return (*this) = (const BigInt &)b;
    }
    deallocate();
    digits = b.digits;
    sign = b.sign;
    b.digits = nullptr;
    return *this;
}

BigInt &BigInt::operator=(const BigInt &b)
{
    if(digits==nullptr){allocate(size);}
    memset(this->digits, (~0) * b.sign, size * sizeof(uint32_t));
    memcpy(this->digits, b.digits, sizeof(uint32_t) * std::min(b.size, size));
    sign = b.sign;
    return *this;
}
std::string BigInt::toDec() const
{
    std::string res;
    int s_size = ((size * 1.204) + 1); //ceiling, 1.204 = log_10(16)
    uint32_t *tab = new uint32_t[s_size];
    memset(tab, 0, s_size * sizeof(uint32_t));
    buffer = 0;

    for (int i = size - 1; i >= 0; --i)
    {
        for (int j = sizeof(uint32_t) * 8 - 1; j >= 0; --j)
        {
            for (int k = 0; k < s_size; ++k)
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
            for (int k = 0; k < s_size; ++k)
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
    for (i = s_size - 1; i >= 0; --i)
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
    for (int i = 0; i < size; i++)
    {
        buffer += (~digits[i]);
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    sign = ((sign == 0) + (buffer != 0)) & 1;
}
uint32_t BigInt::allocate(unsigned int target_size)
{
    if (digits != nullptr)
    {
        return 0;
    }
    digits = new uint32_t[target_size];
    memset(digits, sign * 255, sizeof(uint32_t) * target_size);
    return target_size;
}

void BigInt::clear()
{
    sign = false;
    if (digits != nullptr)
    {
        memset(digits, sign * 255, sizeof(uint32_t) * size);
    }
}

std::string BigInt::toBin() const
{
    std::string result;
    for (int i = 0; i < size; i++)
    {
        result += var2Bin(digits[size - 1 - i]);
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
    for (int i = size - 1; i >= 0; --i)
    {
        if (digits[i] != (~0) * sign)
        {
            int j;
            auto mask = endian32;
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

//
//        COMPARISION OPERATORS
//

bool BigInt::operator<(const BigInt &b) const
{
    bool mySign = sign;
    bool bSign = b.sign;
    if (mySign ^ bSign)
    {
        return mySign;
    }
    for (int i = std::max(size - 1, b.size - 1); i >= 0; --i)
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
    for (int i = size - 1; i >= 0; --i)
    {
        if ((*this)[i] == b[i])
        {
            continue;
        }
        return (!mySign) ^ ((*this)[i] > b[i]);
    }
    return false;
}
bool BigInt::operator==(const BigInt &b) const
{
    for (unsigned int i = 0; i < std::max(size, b.size); i++)
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
    for (unsigned int i = 0; i < std::max(size, b.size); i++)
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
    if (size == 1)
    {
        digits[0] = (digits[0] << 1);
        return;
    }
    uint64_t *ptr;
    for (int i = size - 1; i > 0; i--)
    {
        ptr = (uint64_t *)(digits + i - 1);
        digits[i] = ((*ptr) >> (sizeof(uint32_t) * 8 - 1));
    }
    digits[0] = (digits[0] << 1);
}
void BigInt::shiftRight()
{
    if (size == 1)
    {
        digits[0] = (digits[0] >> 1);
        return;
    }
    uint64_t *ptr;
    for (int i = 0; i < size - 1; i++)
    {
        ptr = (uint64_t *)(digits + i);
        digits[i] = ((*ptr) >> 1);
    }
    digits[size - 1] = (((*ptr) >> (sizeof(uint32_t) * 8) + 1) + endian32 * sign);
}

//
//         ADDITION AND SUBTRACTION BY VALUE
//

BigInt BigInt::operator+(const BigInt &b) const
{
    buffer = 0;
    BigInt r1(std::max(size, b.size));
    for (int i = 0; i < r1.size; i++) // Overflows as intended...
    {
        buffer += digits[i % size] * (i < size);
        buffer += b.digits[i % b.size] * (i < b.size);
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

    for (int i = 0; i < r1.size; i++)
    {
        buffer += r1.digits[i];
        buffer += digits[i % size] * (i < size);
        r1.digits[i] = buffer;
        buffer = (buffer >> (8 * sizeof(uint32_t)));
    }
    r1.sign = ((sign - r1.sign + (buffer != 0)) & 1);
    return r1;
}

//
//         SHIFTS
//

BigInt BigInt::operator<<(int shift) const
{
    BigInt result(*this);
    if (shift <= 0)
    {
        if (shift == 0)
        {
            return result;
        }
        return (result >> (-shift));
    }
    int majorOffset = shift / (sizeof(uint32_t) * 8);
    int minorOffset = (shift - majorOffset * (sizeof(uint32_t) * 8));
    for (int i = size - 1; i >= 0; --i)
    {
        buffer = (result[i - majorOffset - 1] >> (sizeof(uint32_t) * 8 - minorOffset)) * (minorOffset != 0);
        buffer += (result[i - majorOffset] << (minorOffset));
        result.digits[i] = buffer;
    }
    return result;
}
BigInt BigInt::operator>>(int shift) const
{
    BigInt result(*this);
    if (shift <= 0)
    {
        if (shift == 0)
        {
            return result;
        }
        return (result << (-shift));
    }
    int majorOffset = shift / (sizeof(uint32_t) * 8);
    int minorOffset = (shift - majorOffset * (sizeof(uint32_t) * 8));
    for (int i = 0; i < size; ++i)
    {
        buffer = (result[i + majorOffset] >> (minorOffset));
        buffer += (result[i + 1 + majorOffset] << (sizeof(uint32_t) * 8 - minorOffset)) * (minorOffset != 0);
        result.digits[i] = buffer;
    }
    return result;
}
BigInt &BigInt::operator<<=(int shift)
{
    if (shift <= 0)
    {
        if (shift == 0)
        {
            return (*this);
        }
        return ((*this) >>= (-shift));
    }
    int majorOffset = shift / (sizeof(uint32_t) * 8);
    int minorOffset = (shift - majorOffset * (sizeof(uint32_t) * 8));
    for (int i = size - 1; i >= 0; --i)
    {
        buffer = ((*this)[i - majorOffset - 1] >> (sizeof(uint32_t) * 8 - minorOffset)) * (minorOffset != 0);
        buffer += ((*this)[i - majorOffset] << (minorOffset));
        (*this).digits[i] = buffer;
    }
    return (*this);
}
BigInt &BigInt::operator>>=(int shift)
{
    if (shift <= 0)
    {
        if (shift == 0)
        {
            return (*this);
        }
        return ((*this) <<= (-shift));
    }
    int majorOffset = shift / (sizeof(uint32_t) * 8);
    int minorOffset = (shift - majorOffset * (sizeof(uint32_t) * 8));
    for (int i = 0; i < size; ++i)
    {
        buffer = ((*this)[i + majorOffset] >> (minorOffset));
        buffer += ((*this)[i + 1 + majorOffset] << (sizeof(uint32_t) * 8 - minorOffset)) * (minorOffset != 0);
        (*this).digits[i] = buffer;
    }
    return (*this);
}

//
//        NON COPYING ADDITION AND SUBTRACTION
//

BigInt &BigInt::operator+=(const BigInt &b)
{
    b.buffer = std::min(size, b.size);
    buffer = 0;
    int i;
    for (i = 0; i < b.buffer; i++)
    {
        buffer += digits[i];
        buffer += b.digits[i];
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    for (; i < size && buffer != 0; i++)
    {
        buffer += digits[i];
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    sign = ((sign + b.sign + (buffer != 0)) & 1);
    return *this;
}
BigInt &BigInt::operator-=(const BigInt &b)
{
    buffer = (uint32_t)(1);
    for (int i = 0; i < size; i++)
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
    BigInt a(2 * size);
    a = (*this);
    return ((a*b.computeInverse(size-b.size+1)) >> ((size-b.size+1) * 32)) + BigInt("1");
}
BigInt BigInt::operator%(const BigInt &b) const
{
    if (b == 0)
    {
        throw std::exception();
    }
    BigInt buffer = *this;
    BigInt divisor(size);
    divisor = b;
    bool sign = divisor.abs() ^ buffer.abs();
    int s1 = buffer.getActualSize();
    int s2 = divisor.getActualSize();
    divisor <<= (s1 - s2);
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
    for (int i = 0; i < size; i++)
    {
        buffer += (a.buffer * a[i]);
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
}
void BigInt::karatsuba(const BigInt &a, const BigInt &b, BigInt &buff) // Should not modify its parameters
{
    if ((a.size != 0) * (b.size != 0) * (size != 0) == 0)
    {
        this->clear();
        return;
    }
    if ((b.size - 1) * (a.size - 1) == 0)
    {
        if (b.size == 1)
        {
            mulAdd(a, b.digits[0]);
            return;
        }
        mulAdd(b, a.digits[0]);
        return;
    }

    unsigned int mid = std::max(a.size + 1, b.size + 1) / 2;
    // I have guarantee that a1.n>=a2.n and b1.n>=b2.n

    BigInt a1(a, 0, mid);
    BigInt a2(a, a1.size, mid);
    BigInt b1(b, 0, mid);
    BigInt b2(b, b1.size, mid);

    BigInt r1((*this), 0, 2 * mid);
    BigInt r2((*this), mid, size - mid);
    BigInt r3((*this), 2 * mid, size - 2 * mid);

    BigInt buf1(buff, 0, 2 * mid);
    BigInt buf4(buff, buf1.size, buff.size - buf1.size);

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

    buf1.karatsuba(a1, b1, buf4); // two versions optimization ?

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
        a1.digits = a2.digits = b1.digits = b2.digits = r1.digits = r2.digits = r3.digits = buf1.digits = buf4.digits = nullptr;// clean up
        return;
    }
    r2 += buf1;
    a1.digits = a2.digits = b1.digits = b2.digits = r1.digits = r2.digits = r3.digits = buf1.digits = buf4.digits = nullptr;// clean up
}
BigInt BigInt::operator*(const BigInt &_b) const
{
    BigInt result(size);
    BigInt buff1(2*size);
    result.karatsuba(*this, _b, buff1);
    return result;
}
BigInt BigInt::operator*(int32_t b) const
{
    bool bSign = (b < 0);
    b = b * (1 - (2 * bSign));
    BigInt result = *this;
    buffer = 0;
    for (int i = 0; i < size; i++)
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
    //return 1;
    buffer = 1;
    buffer = (buffer << (sizeof(uint64_t) * 8-1)); //good idea- continue
    auto position = (getActualSize() + 31) / 32;
    buffer /= (digits[position - 1]);
    BigInt test(2);
    test.digits[0] = buffer;
    test.digits[1] = (buffer>>(sizeof(uint32_t)*8));
    BigInt mBuff(16);
    mBuff = (*this);

    BigInt a(k);
    BigInt kbuff(4 * k);
    BigInt buff(2 * k);
    BigInt res(2 * k);
    a.digits[0] = buffer;
    a.digits[1] = (buffer>>(sizeof(uint32_t)*8));
    k *= 32;
    a = (a << (k - (position)*32));
    int i;
    for (i = 0; i < k; ++i)
    {
        res = (*this);
        buff.karatsuba(res, a, kbuff);
        res = BigInt("2");
        res = (res << k); //Optimize!
        res -= buff;
        buff = res;
        res.karatsuba(buff, a, kbuff);
        res = (res >> k);
        if (res == a)
        {
            break;
        }
        a = res;
    }
    std::cout << "iterations: " << i << " k " << k << " n: " << size << " a.n: " << a.size << " res.n: " << res.size << std::endl;
    return a;
}