#!/usr/bin/env bash
cd build
# rm -rf ./**
cmake ..
make
cd ..
./build/rubik
