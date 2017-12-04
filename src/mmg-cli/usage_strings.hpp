#ifndef _CLI_USAGE_STRINGS_
#define _CLI_USAGE_STRINGS_

#include <ostream>
#include <string>

namespace usage {
	static const std::string main = "Usage: mmg-cli <mode> <job>[,<job>...] [options]\n";

	static const std::string modes =
		"Modes:\n"
		"  repr     Generate vectors or matrices suitable for distance computations\n"
		"  dist     Compute pairwise distance matrices\n"
		"  resume   Resume an interrupted task\n"
		"  help     Print a help message\n"
		"  version  Print version information\n";

	static const std::string options =
		"Options:\n"
		"  --in <path>\n"
		"     Input file or directory\n"
		"     (in mode 'dist', this produces a symmetric pairwise distance matrix for\n"
		"      every pair of inputs in the given dataset)\n"
		"  --in1, --in2 <path> [only mode 'dist']\n"
		"     Input files or directories\n"
		"     (in mode 'dist', this produces a pairwise distance matrix where rows\n"
		"      correspond to inputs in 'in1' and columns correspond to inputs in 'in2')\n"
		"  --out <path>\n"
		"     Output file or directory\n"
		"  --threads N\n"
		"     Number of CPU threads to use [default: autodetect, all threads]\n"
		//"  --blocksize N [only mode 'dist']\n"
		//"     Number of representations per thread to load into memory\n"
		//"     [default: auto-adjust to use at most 80% of free memory if supported,\n"
		//"               or 2GB if not]\n"
		//"  --no-cuda [only mode 'dist']\n"
		//"     Disable CUDA support\n"
		//"  --no-opencl [only mode 'dist']\n"
		//"     Disable OpenCL support\n"
		"  --int-precision (8|16|32|64)\n"
		"     Integer precision [default: 64]\n"
		"  --float-precision (single|double)\n"
		"     Floating-point precision\n"
		"     [default: single ('dist' mode), double ('repr' mode)]\n";

	static const std::string repr_jobs =
		"Jobs for 'repr' mode:\n"
		"  Input as FASTA files:\n"
		"    cgr-k=N[-freq][-order=XXXX]   (Chaos Game Representation)\n"
		//"    cgr-(k=N|spacedseed=M)[-freq][-order=XXXX]   (Chaos Game Representation)\n"
		//"    krap-(k=N|spacedseed=M)[-order=XXXX]         (k-Relative Abundance Profile)\n"
		//"    cv-(k=N|spacedseed=M)[-order=XXXX]           (Composition Vector)\n"
		"      Options:\n"
		"        k           Length of the substrings to count\n"
		//"        spacedseed  Specification of the spaced seed, as a position bitmask\n"
		"        freq        Use relative frequencies instead of occurrence counts\n"
		"        order       Order of the labels (CW from bottom-left) [default: ACGT]\n"
		"    sparsecgr-k=N[-freq][-order=XXXX]  (Sparse CGR)\n"
		//"    sparsecgr-(k=N|spacedseed=M)[-freq][-order=XXXX][-freqcut=N]  (Sparse CGR)\n"
		//"    sparsekrap-(k=N|spacedseed=M)[-order=XXXX][-freqcut=N]        (Sparse kRAP)\n"
		//"    sparsecv-(k=N|spacedseed=M)[-order=XXXX][-freqcut=N]          (Sparse CV)\n"
		"      Options:\n"
		"        k           Length of the substrings to count\n"
		//"        spacedseed  Specification of the spaced seed, as a position bitmask\n"
		"        freq        Use relative frequencies instead of occurrence counts\n"
		"        order       Order of the labels (CW from bottom-left) [default: ACGT]\n"
		//"        freqcut     Discard substrings with count/frequency lower than threshold\n"
		"    Note: if there are multiple sequences in the same FASTA file, representations\n"
		"          will be computed for each sequence and added elementwise to produce\n"
		"          one output per file.\n"
		"  Input as CGR-like representation:\n"
		"    descr-bins=[N,...]-windows=[N,...]  (Descriptor)\n"
		"      Options:\n"
		"        bins     List of bin values\n"
		"        windows  List of window sizes\n";

	static const std::string dist_jobs =
		"Jobs for 'dist' mode:\n"
		"  Input as any representation:\n"
		"    euclidean           (Euclidean)\n"
		"    sqeuclidean         (Squared Euclidean)\n"
		"    manhattan           (Manhattan)\n"
		"    cosine              (Cosine)\n"
		"    pearson             (Pearson correlation coefficient)\n"
		"    approxinfo[-est=X]  (Approximate Information distance)\n"
		"      Options:\n"
		"        est    Estimation formula (choices: A, B) [default: A]\n"
		"  Input as any dense matrix representation:\n"
		"    ssim  (Structural Similarity Index)\n"
		/*"  Input as FASTA files:\n"
		 *"    usm-est=X-compr=X[-level=N]  (Estimated Universal Similarity Metric)\n"
		 *"      Options:\n"
		 *"        est    Estimation formula (choices: ucd, ncd, ncd2, cd, cdm)\n"
		 *"        compr  Compressor (choices: paq8, zpaq, gzip, bzip2, xz, mfcompress)\n"
		 *"        level  Compression level (if applicable, between 0 and 9)\n"*/
		;

	inline void put_help(std::ostream &stream) {
		using std::endl;
		stream << main << endl << modes << endl << options << endl << repr_jobs << endl << dist_jobs;
	}

	inline void put_options_help(std::ostream &stream) {
		using std::endl;
		stream << main << endl << modes << endl << options;
	}
}

#endif
