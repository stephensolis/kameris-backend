icl /EHsc /nologo /W3 /fast /QxSSE4.1 /I../.. /Fegeneration_cgr_windows_sse41.exe generation_cgr.cpp
icl /EHsc /nologo /W3 /fast /QxAVX /I../.. /Fegeneration_cgr_windows_avx.exe generation_cgr.cpp
icl /EHsc /nologo /W3 /fast /QxCORE-AVX2 /I../.. /Fegeneration_cgr_windows_avx2.exe generation_cgr.cpp

icl /EHsc /nologo /W3 /fast /QxSSE4.1 /I../.. /Fegeneration_dists_windows_sse41.exe generation_dists.cpp
icl /EHsc /nologo /W3 /fast /QxAVX /I../.. /Fegeneration_dists_windows_avx.exe generation_dists.cpp
icl /EHsc /nologo /W3 /fast /QxCORE-AVX2 /I../.. /Fegeneration_dists_windows_avx2.exe generation_dists.cpp

@del *.obj
