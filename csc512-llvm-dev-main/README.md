### ***This dev repository doesn't include test file and script***

# Introduction
This project is an out-of-tree customized LLVM pass that aims to automatically recognize "seminal input features" of the running time of a program. It is primarily divided into two parts, **BranchPointerTrace** and **FeatureDetection**. Since the project depends on the LLVM-project, you would have to set path of llvm before you start running this program.
```bash
$ export LLVM_DIR = llvm/build/path/lib/cmake/llvm
```

## Part 1: Key Points Detection
There are two goals in part 1. The first goal is to create a profiling tool that can make a program output a branch-pointer trace after the execution of a program. The second is to create a binary profiling tool that can make a program
output the total number of executed instructions of that program after the  execution of that program by using Valgrind.

All scripts in part 1 can be executed in project directory and test files are under **testfile**.

### Branch Pointer Trace
The code for this section is mostly in **BranchPointerTrace.cpp** file.

There are two scripts to automated the building and the execution of Branch-Pointer Trace, which is **build.sh** and **runtest.sh**. 

#### Build BranchPointerTrace pass
This script utilizes the **Ninja** to build the pass.
```bash
$ bash build.sh
```
#### Run the testfile of BranchPointerTrace pass
Before you run the testfile, you might have to modify the location of LLVM tool commands such as **clang**, **opt**, and **llc** in runtest.sh, and run it along with the test filename(without .c) that you want to analyze.
```
$ bash runtest.sh forloop
$ bash runtest.sh sort
$ bash runtest.sh floatformat
```

### Valgrind
#### Run the Valgrind tool with test files
```
$ bash valgrind.sh forloop
$ bash valgrind.sh sort
$ bash valgrind.sh floatformat
```

## Part 2: Seminal Input Features Detection
The goal of this part is to build a analysis tool to determine the key points and track its behaviors, which is mostly handled in function **FindKeyPoint**. The result is printed out in a simplified output format, you could use the code that I commented out with a (*) to view the whole progress ofthe IR values.

**void FindKeyPoint**
This function is to traverse and analyze the input values thus tracing back the key points to determine what were the original variables in C program.

**void IsFunctionTakingInput**
As the name of the function says, it checks whether user gives input to the function.

**To build**
```
$ mkdir buildFD
$ cd buildFD
$ cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ../FeatureDetection -G Ninja
$ ninja -C . FeatureDetection
```
**To run**
Please edit the script with the program you want to run directly.
```
$ bash runtest3.sh
```

## Part 3: Integrated Seminal Input Feature Detection
In this section, we combine part1 and part2 and run them through script.
After running part1 and part2, both tools should be built and you only have to run **runtestFinal.sh**.
```
$ bash runtestFinal.sh forloop
$ bash runtestFinal.sh sort
$ bash runtestFinal.sh floatformat
```