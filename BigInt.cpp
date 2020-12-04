#include "BigInt.h"

uint64_t BigInt::endianMask = (uint64_t)1 << (sizeof(uint64_t) * 8 - 1);