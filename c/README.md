# C Implementation

## Installation

This requires the `glib` C library.
Install this with

```shell
sudo apt install libglib2.0-dev
sudo apt install libgtk2.0-dev
```

## Compiling

Compile with

```shell
gcc calculator.c -lm -o calculator `pkg-config --cflags --libs glib-2.0`
```