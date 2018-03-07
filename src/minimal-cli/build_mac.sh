#!/bin/sh

icpc -std=gnu++14 -w3 -diag-disable:remark -fast -xSSE4.1 -I../.. -o generation_cgr_mac_sse41 generation_cgr.cpp /usr/local/opt/boost/lib/libboost_system.a /usr/local/opt/boost/lib/libboost_filesystem.a
icpc -std=gnu++14 -w3 -diag-disable:remark -fast -xAVX -I../.. -o generation_cgr_mac_avx generation_cgr.cpp /usr/local/opt/boost/lib/libboost_system.a /usr/local/opt/boost/lib/libboost_filesystem.a
icpc -std=gnu++14 -w3 -diag-disable:remark -fast -xCORE-AVX2 -I../.. -o generation_cgr_mac_avx2 generation_cgr.cpp /usr/local/opt/boost/lib/libboost_system.a /usr/local/opt/boost/lib/libboost_filesystem.a

icpc -std=gnu++14 -w3 -diag-disable:remark -fast -xSSE4.1 -I../.. -o generation_dists_mac_sse41 generation_dists.cpp /usr/local/opt/boost/lib/libboost_system.a /usr/local/opt/boost/lib/libboost_filesystem.a
icpc -std=gnu++14 -w3 -diag-disable:remark -fast -xAVX -I../.. -o generation_dists_mac_avx generation_dists.cpp /usr/local/opt/boost/lib/libboost_system.a /usr/local/opt/boost/lib/libboost_filesystem.a
icpc -std=gnu++14 -w3 -diag-disable:remark -fast -xCORE-AVX2 -I../.. -o generation_dists_mac_avx2 generation_dists.cpp /usr/local/opt/boost/lib/libboost_system.a /usr/local/opt/boost/lib/libboost_filesystem.a
