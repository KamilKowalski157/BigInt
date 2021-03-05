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
// For double dabble algorithm
char BigInt::conversionLookupTable[256] = {0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                                           0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                                           0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                                           0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                                           0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
                                           48, 48, 48, 48, 48, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51};

char BigInt::hexLookupTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

BigInt::BigInt(const std::string &str) : size(allocate((str.size() / 8) + 1))
{
    sign = false;
    if (str[0] == '-')
    {
        sign = true; // TODO
    }
    uint32_t d_buff;
    if (sign)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0, k = i * 8; j < 8; j++, k++)
            {
                digits[i] += (((str[((str.size() - 2 - k) % (str.size() - 1)) + 1] - '0') * (k < str.size() - 1)) << (j * 4));
            }
        }
    }
    else
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0, k = i * 8; j < 8; j++, k++)
            {
                digits[i] += (((str[(str.size() - 1 - k) % str.size()] - '0') * (k < str.size())) << (j * 4));
            }
        }
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            d_buff = (d_buff >> 1);
            d_buff += (1 << (sizeof(uint32_t) * 8 - 1)) * (digits[i] & 1);

            buffer = 0;
            for (int k = size - 1; k >= i; k--)
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
        if (sign)
        {
            buffer = ~d_buff;
            buffer += (i == 0);
        }
        else
        {
            buffer = d_buff;
        }
        digits[i] = buffer;
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
    else
    {
    }
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
    if (digits == nullptr)
    {
        allocate(size);
    }
    memset(this->digits, (~0) * b.sign, size * sizeof(uint32_t));
    memcpy(this->digits, b.digits, sizeof(uint32_t) * std::min(b.size, size));
    sign = b.sign;
    return *this;
}
std::string BigInt::toDec() const
{
    std::string res;
    int s_size = ((size * 1.205) + 1); //ceiling, 1.204 = log_10(16)
    uint32_t *tab = new uint32_t[s_size];
    memset(tab, 0, s_size * sizeof(uint32_t));
    buffer = 0;

    for (int i = size - 1; i >= 0; --i)
    {
        for (int j = sizeof(uint32_t) * 8 - 1; j >= 0; --j)
        {
            for (unsigned char *ptr = (unsigned char *)tab; ptr < (unsigned char *)(tab + s_size - (int)(i * 1.204)); ++ptr)
            {
                *ptr += conversionLookupTable[*ptr];
            }
            buffer = 0;
            for (int k = 0; k < s_size - (int)(i * 1.204); ++k)
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
    char digit = sign;
    for (int a = 0; a < i + 1; ++a)
    {
        for (int b = 0; b <= 7 * (a != i) + j * (a == i); ++b)
        {
            digit += ((tab[a] >> (b * 4)) % 16);
            res.insert(res.begin(), '0' + (digit % 10));
            digit /= 10;
        }
    }
    if (res.size() == 0)
    {
        res = "0";
    }
    if (sign)
    {
        res.insert(res.begin(), '-');
    }
    delete[] tab;
    return res;
}
std::string BigInt::toHex() const
{
    std::string result;
    for (unsigned char *ptr = (unsigned char *)digits; ptr < (unsigned char *)(digits + size); ++ptr)
    {
        result.insert(result.begin(), hexLookupTable[(*ptr) & 15]);
        result.insert(result.begin(), hexLookupTable[(*ptr) >> 4]);
    }
    return result;
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
    uint32_t new_size = 1;
    while (new_size < target_size)
    {
        new_size <<= 1;
    }
    digits = new uint32_t[new_size];
    memset(digits, sign * 255, sizeof(uint32_t) * new_size);
    return new_size;
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
    return 0;
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
    buffer = 0;
    int i, min_size;
    min_size = std::min(size, b.size);
    for (i = 0; i < min_size; ++i)
    {
        buffer += digits[i];
        buffer += b.digits[i];
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    for (; i < size; ++i)
    {
        buffer += digits[i];
        buffer += ((uint32_t)(~0) * (b.sign == 1));
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    sign = ((sign + b.sign + (buffer != 0)) & 1);
    return *this;
}
BigInt &BigInt::operator-=(const BigInt &b)
{
    buffer = (uint32_t)(1);
    int i, min_size;
    min_size = std::min(size, b.size);
    for (i = 0; i < min_size; ++i)
    {
        buffer += digits[i];
        buffer += ~b.digits[i];
        digits[i] = buffer;
        buffer = (buffer >> (sizeof(uint32_t) * 8));
    }
    for (; i < size; ++i)
    {
        buffer += digits[i];
        buffer += ((uint32_t)(~0) * (b.sign == 0));
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
    BigInt a(size * 2);
    BigInt c(size);
    BigInt aliasa((*this), 0, size);
    BigInt aliasb(b, 0, b.size);
    bool signa = aliasa.abs();
    bool signb = aliasb.abs();
    if (aliasb.getActualSize() == 1)
    {
        aliasa.digits = aliasb.digits = nullptr;
        a = (*this);
        if (signa ^ signb)
        {
            a.negate();
        }
        return a;
    }
    c.computeInverse(aliasb);
    uint32_t shift = (size * 32 - c.buffer);
    a = c * aliasa;
    if (signb)
    {
        aliasb.negate();
    }
    if (signa)
    {
        aliasa.negate();
    }
    if (signa ^ signb)
    {
        a.negate();
    }
    aliasa.digits = aliasb.digits = nullptr;
    return (a >> shift);
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
    BigInt one[2]{1, 1};
    one[0].digits[0] = one[1].digits[0] = 1;
    one[1].negate();
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

void BigInt::naiveMul(const BigInt &a, const BigInt &b)
{
    for (int i = 0, j = 0; i < a.size && i < size; ++i, j = 0)
    {
        buffer = 0;
        for (; (j + i) < size && j < b.size; ++j)
        {
            a.buffer = a.digits[i];
            a.buffer *= b.digits[j];
            buffer += a.buffer;
            buffer += digits[i + j];
            digits[i + j] = buffer;
            buffer = (buffer >> (sizeof(uint32_t) * 8));
        }
        digits[(i + j) % size] += buffer * ((i + j) < size);
    }
}
void BigInt::karatsuba(const BigInt &a, const BigInt &b, BigInt &buff) // Should not modify its parameters
{
    if ((a.size != 0) * (b.size != 0) * (size != 0) == 0)
    {
        return;
    }
    if (size < 128)
    {
        naiveMul(a, b);
        return;
    }

    unsigned int mid = std::max(a.size + 1, b.size + 1) / 2;
    unsigned int mida = std::min(a.size, mid);
    unsigned int midb = std::min(b.size, mid);

    Trickster a1(a.sign, a.digits, mida);
    Trickster a2(a.sign, a.digits + mida, a.size - mida);
    Trickster b1(b.sign, b.digits, midb);
    Trickster b2(b.sign, b.digits + midb, b.size - midb);

    Trickster r1(sign, digits, mid);
    Trickster r2(sign, digits + mid, mid);

    Trickster buff1(buff.sign, buff.digits, 2 * mid);
    Trickster buff2(buff.sign, buff.digits + 2 * mid, 2 * mid);

    r1.bint = a1.bint;
    r1.bint -= a2.bint;

    r2.bint = b1.bint; //!!!!!!
    r2.bint -= b2.bint;

    bool sign_a = false;
    bool sign_b = true;
    if (r1.bint.sign)
    {
        sign_a = true;
        r1.bint.negate();
    }
    if (r2.bint.sign)
    {
        sign_b = false;
        r2.bint.negate();
    }
    buff1.bint.clear();
    buff1.bint.karatsuba(r1.bint, r2.bint, buff2.bint);

    r2.fields.digits = digits + 2 * mid;
    r2.fields.size = size - 2 * mid;

    r1.fields.size = 2 * mid;

    clear();
    r1.bint.karatsuba(a1.bint, b1.bint, buff2.bint);
    r2.bint.karatsuba(a2.bint, b2.bint, buff2.bint);
    buff2.bint.clear();

    if (sign_a ^ sign_b)
    {
        buff.negate();
    }

    buff += r1.bint;
    buff += r2.bint;

    r2.fields.digits = digits + mid;
    r2.fields.size = size - mid;
    r2.bint += buff;
}
BigInt BigInt::operator*(const BigInt &_b) const
{
    BigInt result(size + _b.size);
    BigInt buff1(result.size);

    BigInt aliasa(*this, 0, size);
    BigInt aliasb(_b, 0, _b.size);

    bool signa = aliasa.abs();
    bool signb = aliasb.abs();

    result.karatsuba(aliasa, aliasb, buff1);

    if (signa ^ signb)
    {
        result.negate();
    }
    if (signa)
    {
        aliasa.negate();
    }
    if (signb)
    {
        aliasb.negate();
    }

    aliasa.digits = aliasb.digits = nullptr;
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

//Recursive newton raphson ftw!
void BigInt::computeInverse(const BigInt &c) // Newton-Raphson
{
    if (c.getActualSize() == 0 || size < c.size || size < 2)
    {
        throw std::exception();
    }
    Trickster cMask(c.sign, c.digits, c.size);
    int position = c.getActualSize();
    cMask.bint <<= (c.size * 32 - position);

    buffer = 1;
    buffer = (buffer << (sizeof(uint64_t) * 8 - 1));
    buffer /= (c.digits[c.size - 1]);
    digits[0] = (buffer << 1);
    digits[1] = (buffer >> (sizeof(uint32_t) * 8 - 1));

    BigInt xn2(2 * size);
    BigInt kbuff(4 * size);
    BigInt buff(2 * size);

    Trickster xn1Alias(sign, digits, 2);
    Trickster xn2Alias(xn2.sign, xn2.digits, 4);
    Trickster invAlias(c.sign, c.digits + c.size - 1, 1);
    Trickster buffAlias(buff.sign, buff.digits, 4);
    //std::cout << " k " << size << " n: " << size << " a.n: " << size << " res.n: " << xn2.size << std::endl;

    bool invDouble = false;
    bool basDouble = false;

    int i;
    for (i = 0;; ++i)
    {
        xn2Alias.bint = invAlias.bint;
        buffAlias.bint.clear();
        buffAlias.bint.karatsuba(invAlias.bint, xn1Alias.bint, kbuff);

        xn2Alias.bint.digits[xn1Alias.bint.size] = 2;
        xn2Alias.bint -= buffAlias.bint;
        buffAlias.bint = xn2Alias.bint;
        xn2Alias.bint.clear();
        xn2Alias.bint.karatsuba(buffAlias.bint, xn1Alias.bint, kbuff);

        xn2Alias.bint >>= (xn1Alias.bint.size * 32);
        xn1Alias.bint = xn2Alias.bint;

        if (i & 1)
        {
            invDouble = (invAlias.bint.size != c.size);
            basDouble = (xn1Alias.bint.size != size);
            if (!invDouble && !basDouble)
            {
                break;
            }
            i = -1;
            xn1Alias.fields.size *= (1 + basDouble);
            xn2Alias.fields.size *= (1 + basDouble);
            buffAlias.fields.size *= (1 + basDouble);
            xn1Alias.bint <<= (xn1Alias.bint.size * 16 * basDouble - invAlias.bint.size * 32 * invDouble);
            invAlias.fields.size *= (1 + invDouble);

            invAlias.fields.digits = c.digits + c.size - invAlias.fields.size;
            continue;
        }
    }

    cMask.bint >>= (c.size * 32 - position);
    buffer = (c.size * 32 - position);
}
void BigInt::modExp(const BigInt &base, uint32_t exponent, const BigInt &modulus) // Produces overflow for max value!!!!
{
    clear();
    BigInt pureMul[] = {{size * 2}, {size * 2}};
    BigInt dirtyProd[] = {{size * 2}, {size * 2}};
    dirtyProd[0].digits[0] = pureMul[0].digits[0] = dirtyProd[1].digits[0] = pureMul[1].digits[0] = 1;

    pureMul[1] = pureMul[0] = base;

    BigInt kbuff(size * 4); // karatsuba buffer

    BigInt invBuffer(size * 4);
    BigInt inverse(size * 2);
    inverse.computeInverse(modulus);

    Trickster invBufferAlias(invBuffer.sign, invBuffer.digits, size * 2);

    int a = 0;
    int b = 0;
    for (; exponent != 0; exponent >>= 1, a = !a)
    {
        if (exponent % 2)
        {
            dirtyProd[b].clear();
            dirtyProd[b].karatsuba(pureMul[!a], dirtyProd[!b], kbuff);

            invBuffer.clear();
            invBuffer.karatsuba(inverse, dirtyProd[b], kbuff);
            invBuffer >>= (size * 2 * 32);
            dirtyProd[!b].clear();
            dirtyProd[!b].karatsuba(invBufferAlias.bint, modulus, kbuff);
            dirtyProd[b] -= dirtyProd[!b];
            b = !b;
        }
        pureMul[a].clear();
        pureMul[a].karatsuba(pureMul[!a], pureMul[!a], kbuff);

        invBuffer.clear();
        invBuffer.karatsuba(inverse, pureMul[a], kbuff);
        invBuffer >>= (size * 2 * 32);
        pureMul[!a].clear();
        pureMul[!a].karatsuba(invBufferAlias.bint, modulus, kbuff);
        pureMul[a] -= pureMul[!a];
    }
    (*this) = dirtyProd[!b];
}
void BigInt::pow(const BigInt &base, uint32_t exponent)
{
    clear();
    BigInt pureMul[] = {{size}, {size}};
    BigInt dirtyProd[] = {{size}, {size}};
    dirtyProd[0].digits[0] = pureMul[0].digits[0] = dirtyProd[1].digits[0] = pureMul[1].digits[0] = 1;

    pureMul[1] = pureMul[0] = base;

    BigInt kbuff(size * 2); // karatsuba buffer

    int a = 0;
    int b = 0;
    for (; exponent != 0; exponent >>= 1, a = !a)
    {
        if (exponent % 2)
        {
            dirtyProd[b].clear();
            dirtyProd[b].karatsuba(pureMul[!a], dirtyProd[!b], kbuff);
            b = !b;
        }
        pureMul[a].clear();
        pureMul[a].karatsuba(pureMul[!a], pureMul[!a], kbuff);
    }
    (*this) = dirtyProd[!b];
}