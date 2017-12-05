# modmap-generator-cpp

### Build Status

| Travis <br> (Ubuntu zesty, macOS) | AppVeyor <br> (Windows) | Shippable <br> (Debian sid) | Wercker <br> (Alpine Linux, Arch, Fedora) |
|:-----:|:-----:|:-----:|:-----:|
| [![Travis](https://travis-ci.org/stephensolis/modmap-generator-cpp.svg?branch=master)](https://travis-ci.org/stephensolis/modmap-generator-cpp) | [![AppVeyor](https://ci.appveyor.com/api/projects/status/2t4ydkdixbc00ybv?svg=true)](https://ci.appveyor.com/project/stephensolis/modmap-generator-cpp) | [![Shippable](https://api.shippable.com/projects/58636e44b1bb441000bae2bc/badge?branch=master)](https://app.shippable.com/projects/58636e44b1bb441000bae2bc) | [![Wercker](https://app.wercker.com/status/bc733073537cd665d597942188c5a4ff/s/master)](https://app.wercker.com/project/byKey/bc733073537cd665d597942188c5a4ff) |

| Coverity | Codacy | Coveralls | Codecov |
|:-----:|:-----:|:-----:|:-----:|
| [![Coverity](https://scan.coverity.com/projects/11296/badge.svg)](https://scan.coverity.com/projects/stephensolis-modmap-generator-cpp) | [![Codacy](https://api.codacy.com/project/badge/Grade/7d0d5cf06d1a4b279b2d4a2e47e51e65)](https://www.codacy.com/app/stephensolis/modmap-generator-cpp) | [![Coveralls](https://coveralls.io/repos/github/stephensolis/modmap-generator-cpp/badge.svg?branch=master)](https://coveralls.io/github/stephensolis/modmap-generator-cpp?branch=master) | [![Codecov](https://codecov.io/gh/stephensolis/modmap-generator-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/stephensolis/modmap-generator-cpp) |

This is a fast C++ implementation of some functionality for generating Molecular Distance Maps, as seen in the following papers:

- [Mapping the Space of Genomic Signatures](http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0119815)
- [An investigation into inter- and intragenomic variations of graphic genomic signatures](http://bmcbioinformatics.biomedcentral.com/articles/10.1186/s12859-015-0655-4)
- [Additive methods for genomic signatures](http://bmcbioinformatics.biomedcentral.com/articles/10.1186/s12859-016-1157-8)

It supports the following representations of DNA sequences:

- [FCGR](http://www.ncbi.nlm.nih.gov/pmc/articles/PMC330698/)
- [Descriptors](http://bmcbioinformatics.biomedcentral.com/articles/10.1186/s12859-015-0655-4)

and the following distances:

- [Euclidean](http://en.wikipedia.org/wiki/Euclidean_distance)
- [Squared Euclidean](http://en.wikipedia.org/wiki/Euclidean_distance#Squared_Euclidean_distance)
- [Manhattan](http://en.wikipedia.org/wiki/Manhattan_distance)
- [Cosine](https://en.wikipedia.org/wiki/Cosine_similarity)
- [Pearson Correlation](http://mathworld.wolfram.com/CorrelationCoefficient.html)
- [SSIM](https://ece.uwaterloo.ca/~z70wang/research/ssim/)
- [Approximate Information Distance](http://arxiv.org/abs/cs/0111054)

In order to produce graphical Molecular Distance Maps from the output of this program, you can use [stephensolis/modmap-toolkit](https://github.com/stephensolis/modmap-toolkit) (recommended) or [stephensolis/modmap-generator-mma](https://github.com/stephensolis/modmap-generator-mma) (older).

## Getting Started

Pre-built binaries for Windows, Linux, and macOS [are available here](https://github.com/stephensolis/modmap-generator-cpp/releases/latest).

For best performance, make sure you download the version corresponding to the latest instruction set your CPU supports (if unsure, you can check with [CPU-Z](http://www.cpuid.com/softwares/cpu-z.html), or just pick the SSE3 version and sacrifice performance).

Try putting some FASTA files in a directory `fasta`, creating a directory `output`, then running:

	mmg-cli repr cgr-k=9 --in fasta --out output
	mmg-cli dist manhattan --in output/xxxxx --out output

## Directory Structure

- `libmmg`: A reusable header-only library for working with the representations and distances of this project
	- `distances`: Distance computation for dense vectors/matrices
	- `distances-sparse`: Distance computation for sparse vectors/matrices
	- `io`: Reading and writing FASTA sequences and binary-encoded representations
	- `representations`: Generating representations (FCGR, Descriptors, ...)
	- `utils`: Other useful tools (parallel execution, timing, etc.)
- `src`: The program
	- `common`: Code independent of user interface
	- `mmg-cli`: The command-line interface
- `tests`: The test suite
- `benchmarks`: Used to generate the benchmark results below
- `external`: External libraries and code

## Benchmarks

Below, 'Mathematica' means [stephensolis/modmap-generator-mma](https://github.com/stephensolis/modmap-generator-mma) at commit [212d0fda](https://github.com/stephensolis/modmap-generator-mma/tree/212d0fda91f58e7e7111ef5bfdc1dde0810b8c74), and 'C++' means this repository at commit [8af2864a](https://github.com/stephensolis/modmap-generator-cpp/tree/8af2864a7db76daaa29de3d60585b70e7e6e37fd).

Tests were performed on an AWS [c4.4xlarge](https://aws.amazon.com/ec2/instance-types/#c4) (8 cores/16 threads of an Intel Xeon E5-2666 v3), using Mathematica 11.0.1 and the AVX2-release version of this software. Benchmarking code can be found in the `benchmarks/` folder.

| Test                             | C++     | Mathematica | Speedup |
|----------------------------------|---------|-------------|---------|
| CGR                              | 0.704s  | 36.95s      | 52.5x   |
| Descriptors                      | 0.0169s | 2.253s      | 133x    |
| Euclidean                        | 0.158s  | 2.920s      | 18.5x   |
| Manhattan                        | 0.139s  | 2.871s      | 20.7x   |
| Approximate Information Distance | 0.229s  | 27.20s      | 119x    |
| Cosine                           | 0.154s  | 2.500s      | 16.2x   |
| Pearson                          | 0.361s  | 8.309s      | 23.0x   |
| SSIM                             | 14.27s  | 132.3s      | 9.27x   |

## Building

This project requires:

- a C++14-compatible compiler
- a fairly recent [Boost](http://www.boost.org/) (at least 1.59)
- [CMake](https://cmake.org/) (at least 2.8.12)
- if you wish to run the full test suite: [clang-format](http://clang.llvm.org/docs/ClangFormat.html), [clang-tidy](http://clang.llvm.org/extra/clang-tidy/), [LCOV](http://ltp.sourceforge.net/coverage/lcov.php), `diff`, `grep`, and `perl`

The following compilers have been tested:

| Compiler                | Version                                                              |
|-------------------------|----------------------------------------------------------------------|
| Microsoft Visual Studio | 2015 (19.0), 2017 (19.1)                                             |
| Intel C++ Compiler      | 2017 (17.0), 2018 (18.0)                                             |
| GCC                     | 5.3, 5.4, 6.2, 6.3, 6.4, 7.1                                         |
| Clang                   | 3.8, 3.9, 4.0, 5.0, 6.0 <br> Apple LLVM 6.1, 7.0, 7.3, 8.0, 8.1, 9.0 |

### To compile:

Create a new directory, and inside the directory run `cmake (<options>) <path to source code>` followed by `make` (on a Unix platform) or `nmake` (on Windows).

On Windows, the default Visual Studio generators are untested and may not work. Use the NMake generator (`-G"NMake Makefiles"`) instead.

You can set `-DCMAKE_BUILD_TYPE=` to one of the following possible values:

- `Debug` (default): build a single binary `mmg-cli` in debug mode (including debugging information)
- `Release`: build a single binary `mmg-cli` in release mode
- `ReleaseIntel`: use Intel's C++ compiler (`icl`/`icpc`) to build five separate binaries, each optimized for a different platform:
	- `mmg-cli-sse3`: optimized for SSE3
	- `mmg-cli-sse41`: optimized for SSE4.1
	- `mmg-cli-avx`: optimized for AVX
	- `mmg-cli-avx2`: optimized for AVX2
	- `mmg-cli-avx512`: optimized for AVX-512
- `Coverage`: used to compute line coverage for tests with the `coverage` target (see below)

On Windows when not using MSVC (eg. Intel's compiler with `-DCMAKE_BUILD_TYPE=ReleaseIntel`), you likely need to set `-DBOOST_ROOT=`, `-DBOOST_LIBRARYDIR=`, and `-DBoost_COMPILER=`, for example as follows:

	cmake -G"NMake Makefiles" -DBOOST_ROOT=C:\boost -DBOOST_LIBRARYDIR=C:\boost\lib64-msvc-14.1 -DBoost_COMPILER=-vc141

### To run automated tests:

First run `cmake` as specified above, then run `make <target>` (on a Unix platform) or `nmake <target>` (on Windows), where `<target>` may be:

- `check` or `check-all`: runs all tests listed below except `coverage`
- `check-tests`: runs the test suite from the `tests` directory
- `check-format`: runs code style checks (_this requires `clang-format`, `diff`, and `perl`_)
- `check-lint`: runs static analysis checks (_this requires `clang-tidy`, `grep`, and `perl`_)
- `coverage`: computes line coverage for the tests in the `tests` directory, saving the result in [LCOV](http://ltp.sourceforge.net/coverage/lcov.php) format as `coverage.info` (_this requires GCC as the compiler, `lcov`, and setting `-DCMAKE_BUILD_TYPE=Coverage`_)

## Usage

	Usage: mmg-cli <mode> <job>[,<job>...] [options]

	Modes:
	  repr     Generate vectors or matrices suitable for distance computations
	  dist     Compute pairwise distance matrices
	  resume   Resume an interrupted task
	  help     Print a help message
	  version  Print version information

	Options:
	  --in <path>
	     Input file or directory
	     (in mode 'dist', this produces a symmetric pairwise distance matrix for
	      every pair of inputs in the given dataset)
	  --in1, --in2 <path> [only mode 'dist']
	     Input files or directories
	     (in mode 'dist', this produces a pairwise distance matrix where rows
	      correspond to inputs in 'in1' and columns correspond to inputs in 'in2')
	  --out <path>
	     Output file or directory
	  --threads N
	     Number of CPU threads to use [default: autodetect, all threads]
	  --int-precision (8|16|32|64)
	     Integer precision [default: 64]
	  --float-precision (single|double)
	     Floating-point precision
	     [default: single ('dist' mode), double ('repr' mode)]

	Jobs for 'repr' mode:
	  Input as FASTA files:
	    cgr-k=N[-freq][-order=XXXX]   (Chaos Game Representation)
	      Options:
	        k           Length of the substrings to count
	        freq        Use relative frequencies instead of occurrence counts
	        order       Order of the labels (CW from bottom-left) [default: ACGT]
	    sparsecgr-k=N[-freq][-order=XXXX]  (Sparse CGR)
	      Options:
	        k           Length of the substrings to count
	        freq        Use relative frequencies instead of occurrence counts
	        order       Order of the labels (CW from bottom-left) [default: ACGT]
	    Note: if there are multiple sequences in the same FASTA file, representations
	          will be computed for each sequence and added elementwise to produce
	          one output per file.
	  Input as CGR-like representation:
	    descr-bins=[N,...]-windows=[N,...]  (Descriptor)
	      Options:
	        bins     List of bin values
	        windows  List of window sizes

	Jobs for 'dist' mode:
	  Input as any representation:
	    euclidean           (Euclidean)
	    sqeuclidean         (Squared Euclidean)
	    manhattan           (Manhattan)
	    cosine              (Cosine)
	    pearson             (Pearson correlation coefficient)
	    approxinfo[-est=X]  (Approximate Information distance)
	      Options:
	        est    Estimation formula (choices: A, B) [default: A]
	  Input as any dense matrix representation:
	    ssim  (Structural Similarity Index)

## License ![License](http://img.shields.io/:license-mit-blue.svg)

Note: the files in the `external` directory are licensed as specified in their respective header comments.

    The MIT License (MIT)

    Copyright (c) 2017 Stephen

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
