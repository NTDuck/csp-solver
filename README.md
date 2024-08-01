# csp-solver
A linear C++17-compliant solution to a common variant of Constraint Satisfaction Problems (CSP).

## Objective
The program aims to solve a common variant of CSP colloquially known as "Crack the Lock" or "Crack the Code", an example of which could be found [here](https://puzzling.stackexchange.com/questions/46871/crack-the-lock-code).

![](example.jpg)

## Usage
Typically, user input or configurations are provided via keyboard, command line arguments, or a configuration file. In this project, everything is done at compile-time (except, of course, outputing to the stream buffer), so you'll need to use macros instead. For detailed instructions, please refer to [src/main.cpp](src/main.cpp).

## Build and run
```sh
$ g++ -std=c++17 src/main.cpp -o csp
$ ./csp.exe
With 5 constraints:
(1) 682 has 1 correct digit(s), 1 of which correctly placed.
(2) 614 has 1 correct digit(s), 0 of which correctly placed.
(3) 206 has 2 correct digit(s), 0 of which correctly placed.
(4) 738 has 0 correct digit(s), 18446744073709551615 of which correctly placed.
(5) 780 has 1 correct digit(s), 0 of which correctly placed.
Found 1 matching combinations:
042 
(generated within compile-time, printed in 723ms)
```
(ignore the `18446744073709551615`; it stands for `std::size_t(-1)`, which means that the corresponding part of the Constraint is skipped)