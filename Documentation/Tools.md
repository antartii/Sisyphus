# Tools
This document contains the tools used, their versions, and why they are used as well as some notes about them.

Thoses informations could be used as an argument to switch to a newer/older version if necessary.

## CMake 3.12
CMake 3.12 is used to build or C library and useful when coding and running the program with different OS. Since our project target at least Linux and Windows OS it was the obvious choice.

We're currently using the version 3.12 of cmake to use the project version feature, nothing more at the moment.

The only policy used is `CMP0048` as `NEW` to set `VERSION` via the `project()` function.

## C99
We're currently making this library in `C99`, the only reason being : it's well known, still used, and vulkan was made as a `C99` api.

## Github
[Github](https://github.com/) is a well known platform that help developpers storing, managing and viewing code. And we are no exception and use it for our code.
