# What this is

This is my list of "hello world"s, programs that are used to introduce one to a language

I do this by creating a program, in each language, that can calculate primes with the following features:

1. It is multithreaded, to improve the process
2. It uses a file to store the primes, but can still use a cache in memory

While multithreading and files are not all that one can do with a language (possibly excluding servers, CLI utils, etc), it can serve as a starting position, and one better than printing to standard output
It forces me to be familiar with the syntax and learn how to think in the language, along with learning how to read another set of documentation

# How to build/use

## C++

The C++ one is rather important, as it contains the helper program 'generatestarterprimefile'

To compile the C++, navigate to the C++ directory and run `g++ -pthread primegenerator.cpp -o primegenerator` for the prime generator, and `g++ generatestarterprimefile.cpp -o generatestarterprimefile` for the helper utility

## Rust

Navigate to the directory and run `cargo build`
