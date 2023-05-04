<h1 align="center">AI - CSP snake probelm</h1>


## Table of Contents

- [Build](#install)
- [Usage](#usage)
- [Implementation](#implementation)
- [Dependencies](#dependencies)

## Build
To build and run application execute the following commands.

```shell
g++ --std=c++11 main.cpp -o csp.out
```

To run the input case

```shell
./csp.out in1.txt 
```


## Usage

## Implementation
```util``` namespace contains some utility functios to use during the CSP implmentation such as checking if the position is valid on board, getting neigbors on grid and so on.
```backtrack``` function is the main function to find the solution. it recursively tries to finc solution. In each recursion
it filters out partiall assigned grid if it alreadt violates the condition thus reducing the search space. In addition it reduces the domain for each variable using AC3(Arc consistency) algorithm.


## Dependencies
The project uses the gnu c++ compiler and no external dependencies is used. During testing i have compiled the code
with c++11 standards.