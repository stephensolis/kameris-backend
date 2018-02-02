#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>

#include "filesystem_checks.hpp"

using namespace std;
namespace fs = boost::filesystem;

void check_directory_input(const string &dirname) {
	try {
		fs::path path(dirname);

		if (!fs::exists(path)) {
			throw invalid_argument("Input path '" + dirname + "' does not exist");
		}
		if (!fs::is_directory(path)) {
			throw invalid_argument("Input path '" + dirname + "' must be a directory");
		}
	} catch (const fs::filesystem_error &err) {
		throw invalid_argument("Failed to process input directory '" + dirname + "': " + err.what());
	}
}

void check_directory_output(const string &dirname) {
	try {
		fs::path path(dirname);

		if (!fs::exists(path)) {
			try {
				fs::create_directories(path);
			} catch (const fs::filesystem_error &err) {
				throw invalid_argument("Failed to create output directory '" + dirname + "': " + err.what());
			}
		} else if (!fs::is_directory(path)) {
			throw invalid_argument("Output path '" + dirname + "' must be a directory");
		}
	} catch (const fs::filesystem_error &err) {
		throw invalid_argument("Failed to process output directory '" + dirname + "': " + err.what());
	}
}

void check_file_input(const string &filename) {
	try {
		fs::path path(filename);

		if (!fs::exists(path)) {
			throw invalid_argument("Input path '" + filename + "' does not exist");
		}
		if (!fs::is_regular_file(path)) {
			throw invalid_argument("Input path '" + filename + "' must be a file");
		}
	} catch (const fs::filesystem_error &err) {
		throw invalid_argument("Failed to process input file '" + filename + "': " + err.what());
	}
}

void check_file_output(const string &filename) {
	try {
		fs::path path(filename);

		if (!fs::exists(path.parent_path())) {
			throw invalid_argument("Directory '" + path.parent_path().string() + "' for output file '" +
				path.filename().string() + "' does not exist");
		}
		if (fs::exists(path)) {
			throw invalid_argument("Output file '" + filename + "' already exists");
		}
	} catch (const fs::filesystem_error &err) {
		throw invalid_argument("Failed to process output file '" + filename + "': " + err.what());
	}
}
