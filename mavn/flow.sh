#!/bin/bash

# Do not execute but copy-paste commands in terminal.
exit 0

# First time on this computer to install stuff.
sudo ./waf prerequisites

# Just first time after download/unpacking.
./waf configure

# On every code change do build.
./waf build

# On every change of mavn file or build run program
./build/mavn test/simple.mavn

