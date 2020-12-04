#include <iostream>
#include "BigInt.h"
#include "Tester.h"

int main(int argc, char ** argv)
{
    //BigInt a(-25);
    //std::cout<<"magical number: "<<a<<std::endl;
    Tester tester;
    tester.testSubtraction(1000000);
    return 0;
}