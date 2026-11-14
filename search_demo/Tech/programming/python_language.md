# Getting started with Python

Python is a high level programming language known for its readable syntax. Blocks of code are defined by indentation instead of braces, and variables do not need a declared type.

To keep the dependencies of each project separate, create a virtual environment with python -m venv .venv, activate it and install packages with pip. A requirements file lists the exact versions so that other developers can reproduce the same environment.

For numerical work most people use numpy, which stores numbers in compact arrays and runs the heavy loops in optimized C code. Multiplying a large matrix by a vector in numpy is often hundreds of times faster than writing the same loop in pure Python.
