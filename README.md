# BigInt
Proof of concept integer arithmetic 
# About
Library intended to provide fast arithmetic across arbitrarily large integers. Repository also contains `Tester` class intended to check 
performance and validity of particular operations. As of now range of operations is limited to basic addition, subtraction, multiplication and division
# Operations
 ## Addition&Subtraction
 Implemented based on simple for loop with int64_t carry\
 In later stages plan to try fixing `BigInt` size and unrolling loops for better performance, also using pointers in for?
 ## Multiplication
 Simple karatsuba implementation with buffer\
 Possible improvement might be to change length at which arguments are split (`mid`)\
 Time complexity of <img src="https://latex.codecogs.com/png.latex?{\color{white}O(n^{log_23})}" title="{\color{white}O(n^{log_23})}" />
 ## Division
 Simple non restoring division, alternatively Newton-Raphson division using karatsuba as a base operation
# TODO
- Add Doxygen documentation
- Implement double dabble algorithm to convert to base 10
- Implement modulo
- Add template with digit length as argument
- Create python script to plot performance complexity for different operations
