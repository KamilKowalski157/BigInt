#include <iostream>
#include "BigInt.h"
#include "Tester.h"

int main(int argc, char ** argv)
{
    Tester tester;
    tester.testShift(100000000);
    return 0;
}