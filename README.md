# Intermodulation Distortion Calculator

A tool to calculate imtermodulation distortion.

The implementation has some interesting tricks that I'll add comments about at a later date in the code and this README.

This implementation takes advantage of NUMBA JIT, compiling the recursive functions in order to execute faster. 
It is more than 10 times faster than pure Python. 