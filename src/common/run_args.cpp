#include <cstdint>
#include <istream>
#include <ostream>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/boost_optional.hpp>
#include <cereal/types/boost_variant.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

#include "options_structs.hpp"
#include "run_args.hpp"

using namespace std;
using namespace boost;

namespace cereal {
	void save(cereal::PortableBinaryOutputArchive &archive, const bool &val) {
		archive(static_cast<uint8_t>(val));
	}
	void load(cereal::PortableBinaryInputArchive &archive, bool &val) {
		uint8_t store_val;
		archive(store_val);
		val = !!store_val;
	}
}


run_args read_run_args(istream &stream) {
	cereal::PortableBinaryInputArchive input_archive(stream);

	run_args data;
	input_archive(data);
	return data;
}

void write_run_args(ostream &stream, const run_args &options) {
	cereal::PortableBinaryOutputArchive output_archive(stream);

	output_archive(options);
}
