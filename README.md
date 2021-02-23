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
 Time complexity of <img src="https://latex.codecogs.com/png.latex?{\color{white}O(n^{log_23})}" title="{\color{white}O(n^{log_23})}" />\
 Plots confirm complexity in range 1024-131,072 32-bit digits as a factors, (using python plotter look for coefficient parameter k, complexity then is calculated as: <img src="https://latex.codecogs.com/gif.latex?O(n^{\log_2k}{\color{White}}" title="O(n^{\log_2k}){\color{White}}" />)
 ## Division
 Newton-Raphson division using karatsuba as a base operation, according to not-so-reliable plots it achieves slightly worse complexity than karatsuba
 ## Conversion to base-10
 O(n^2) complexity, using double dabble algorithm, possible idea is to use 8 as a multiplier and 1024 as base (1000%8 = 1) 
 allowing for more efficient memory management and shorter execution time
 
# TODO
- [ ] Add Doxygen documentation
- [x] Implement double dabble algorithm to convert to and from base 10
- [x] Implement modulo
- [x] Create python script to plot performance complexity for different operations
