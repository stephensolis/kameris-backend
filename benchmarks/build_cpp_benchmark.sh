#!/bin/sh

icpc -std=gnu++14 -w3 -diag-disable:remark -fast -static -xCORE-AVX2 -I.. -o benchmark benchmark.cpp -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
