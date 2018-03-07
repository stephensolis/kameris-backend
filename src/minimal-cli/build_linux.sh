#!/bin/sh

icpc -std=gnu++14 -w3 -diag-disable:remark -fast -static -xSSE4.1 -I../.. -o generation_cgr_linux_sse41 generation_cgr.cpp -lboost_system -lboost_filesystem -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
icpc -std=gnu++14 -w3 -diag-disable:remark -fast -static -xAVX -I../.. -o generation_cgr_linux_avx generation_cgr.cpp -lboost_system -lboost_filesystem -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
icpc -std=gnu++14 -w3 -diag-disable:remark -fast -static -xCORE-AVX2 -I../.. -o generation_cgr_linux_avx2 generation_cgr.cpp -lboost_system -lboost_filesystem -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

icpc -std=gnu++14 -w3 -diag-disable:remark -fast -static -xSSE4.1 -I../.. -o generation_dists_linux_sse41 generation_dists.cpp -lboost_system -lboost_filesystem -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
icpc -std=gnu++14 -w3 -diag-disable:remark -fast -static -xAVX -I../.. -o generation_dists_linux_avx generation_dists.cpp -lboost_system -lboost_filesystem -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
icpc -std=gnu++14 -w3 -diag-disable:remark -fast -static -xCORE-AVX2 -I../.. -o generation_dists_linux_avx2 generation_dists.cpp -lboost_system -lboost_filesystem -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
