#ifndef _LIBMMG_IO_FASTA_
#define _LIBMMG_IO_FASTA_

#include <istream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

namespace mmg {
	inline std::vector<std::string> read_fasta(std::istream &stream) {
		std::vector<std::string> result;

		std::string curr_line;
		std::string curr_seq;
		while (std::getline(stream, curr_line)) {
			boost::trim(curr_line);
			if (curr_line.empty() || curr_line[0] == '>') {
				if (!curr_seq.empty()) {
					result.push_back(curr_seq);
					curr_seq.clear();
				}
			} else {
				curr_seq += curr_line;
			}
		}

		if (!curr_seq.empty()) {
			result.push_back(curr_seq);
		}

		return result;
	}
}

#endif
