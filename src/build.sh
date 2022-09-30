#!/bin/bash

# Create objects
g++ -c jhash.cpp -o obj/jhash.o

# create library
ar rvs lib/jhash obj/jhash.o

# Build for x86_64 windows
x86_64-w64-mingw32-g++ main.cpp lib/jhash -o bin/jhash.exe -std=c++20 -static-libgcc -static-libstdc++

# Build for linux
g++ main.cpp lib/jhash -o bin/jhash

# Cleanup
rm -rf obj

