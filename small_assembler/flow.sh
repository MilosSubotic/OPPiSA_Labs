#!/bin/bash

exit 0

# Just first time.
./waf configure

# Just to build.
./waf build

# List available ELF/EXE files
ls build/

# Execute example
./build/1_s

# Helper: On every code change build and execute example.
./waf build && ./build/1_s

