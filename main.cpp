#include <iostream>
#include "BigInt.h"
#include "Tester.h"

int main(int argc,char ** argv)
{
    Tester tester;
    tester.pickYourFighter(argv+1,argc-1);
    return 0;
}