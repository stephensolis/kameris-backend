#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <typeinfo>

//ugly workaround for clang-cl on Windows
#if defined(__clang__) && defined(_MSC_VER)
#include <boost/config.hpp>
#define __GNUC__
#include <boost/typeof/typeof.hpp>
#undef __GNUC__
#endif

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/tokenizer.hpp>

#include "../common/defaults.hpp"
#include "../common/options_structs.hpp"
#include "../common/output_headers.hpp"

#include "exceptions.hpp"
#include "streams.hpp"
#include "usage_strings.hpp"

using namespace std;
using namespace boost;
namespace x3 = boost::spirit::x3;

//definitions are like this to avoid use of Boost.Typeof and Boost.Preprocessor,
//which don't work with Intel's compiler on Windows
BOOST_FUSION_ADAPT_STRUCT( //
	cgrlike_options, //
	(decltype(cgrlike_options().repr), repr) //
	(decltype(cgrlike_options().k_or_spacedseed), k_or_spacedseed) //
	(decltype(cgrlike_options().use_freq), use_freq) //
	(decltype(cgrlike_options().order), order))
BOOST_FUSION_ADAPT_STRUCT( //
	sparse_cgrlike_options, //
	(decltype(sparse_cgrlike_options().repr), repr) //
	(decltype(sparse_cgrlike_options().k_or_spacedseed), k_or_spacedseed) //
	(decltype(sparse_cgrlike_options().use_freq), use_freq) //
	(decltype(sparse_cgrlike_options().order), order) //
	(decltype(sparse_cgrlike_options().freqcut), freqcut))
BOOST_FUSION_ADAPT_STRUCT( //
	descr_options, //
	(decltype(descr_options().bins), bins) //
	(decltype(descr_options().windows), windows))

BOOST_FUSION_ADAPT_STRUCT( //
	otherdist_options, //
	(decltype(otherdist_options().dist), dist))
BOOST_FUSION_ADAPT_STRUCT( //
	aid_options, //
	(decltype(aid_options().formula), formula))
BOOST_FUSION_ADAPT_STRUCT( //
	usm_options, //
	(decltype(usm_options().est), est) //
	(decltype(usm_options().compr), compr) //
	(decltype(usm_options().level), level))

namespace parsers {
	/*
	 * Common parsers
	 */
	const auto binary_val = ('0' > x3::attr(false)) | ('1' > x3::attr(true));
	const auto nonnegative_double = x3::real_parser<double, x3::ureal_policies<double>>{};

	/*
	 * 'repr'-mode parsers
	 */
	const auto cgr_start = x3::lit("cgr") > x3::attr(repr_type::cgr);
	const auto krap_start = x3::lit("krap") > x3::attr(repr_type::krap);
	const auto cv_start = x3::lit("cv") > x3::attr(repr_type::cv);
	const auto sparsecgr_start = x3::lit("sparsecgr") > x3::attr(repr_type::cgr);
	const auto sparsekrap_start = x3::lit("sparsekrap") > x3::attr(repr_type::krap);
	const auto sparsecv_start = x3::lit("sparsecv") > x3::attr(repr_type::cv);
	const auto cgrlike_k_or_spacedseed = (x3::lit("-k=") > x3::uint_) | (x3::lit("-spacedseed=") > +binary_val);
	const auto cgr_freq = (x3::lit("-freq") > x3::attr(true)) | x3::attr(false);
	const auto cgrlike_order_str = x3::repeat(2)[x3::upper] | x3::repeat(4)[x3::upper];
	const auto cgrlike_order = (x3::lit("-order=") > cgrlike_order_str) | x3::attr(defaults::cgr_order);
	const auto sparse_cgrlike_freqcut = (x3::lit("-freqcut=") > nonnegative_double) | x3::attr(double(0));

	const auto cgr_jobspec = cgr_start > cgrlike_k_or_spacedseed > cgr_freq > cgrlike_order;
	const auto krap_cv_jobspec = //
		(krap_start | cv_start) > cgrlike_k_or_spacedseed > x3::attr(false) > cgrlike_order;
	const auto sparsecgr_jobspec = //
		sparsecgr_start > cgrlike_k_or_spacedseed > cgr_freq > cgrlike_order > sparse_cgrlike_freqcut;
	const auto sparsekrap_cv_jobspec = //
		(sparsekrap_start | sparsecv_start) //
		> cgrlike_k_or_spacedseed > x3::attr(false) > cgrlike_order > sparse_cgrlike_freqcut;

	const auto descr_bins_list = nonnegative_double % ',';
	const auto descr_windows_list = x3::uint_ % ',';
	const auto descr_jobspec = //
		x3::lit("descr") > "-bins=" > '[' > descr_bins_list > ']' > "-windows=" > '[' > descr_windows_list > ']';

	const auto repr_jobspec_def = //
		cgr_jobspec | krap_cv_jobspec | sparsecgr_jobspec | sparsekrap_cv_jobspec | descr_jobspec;
	const auto repr_jobspec = x3::rule<struct repr_jobspec_class, repr_options, true>{} = //
		x3::eps > repr_jobspec_def > x3::eoi;

	/*
	 * 'dist'-mode parsers
	 */
	struct dist_mode_class : x3::symbols<dist_type> {
		dist_mode_class() {
			add //
				("euclidean", dist_type::euclidean) //
				("sqeuclidean", dist_type::sqeuclidean) //
				("manhattan", dist_type::manhattan) //
				("cosine", dist_type::cosine) //
				("pearson", dist_type::pearson) //
				("ssim", dist_type::ssim);
		}
	} dist_mode;
	struct aid_est_formula_class : x3::symbols<aid_formula> {
		aid_est_formula_class() {
			add //
				("A", aid_formula::A) //
				("B", aid_formula::B);
		}
	} aid_est_formula;
	struct usm_est_formula_class : x3::symbols<usm_formula> {
		usm_est_formula_class() {
			add //
				("ucd", usm_formula::ucd) //
				("ncd", usm_formula::ncd) //
				("ncd2", usm_formula::ncd2) //
				("cd", usm_formula::cd) //
				("cdm", usm_formula::cdm);
		}
	} usm_est_formula;
	struct usm_compr_mode_class : x3::symbols<usm_compressor> {
		usm_compr_mode_class() {
			add //
				("paq8", usm_compressor::paq8) //
				("zpaq", usm_compressor::zpaq) //
				("gzip", usm_compressor::gzip) //
				("bzip2", usm_compressor::bzip2) //
				("xz", usm_compressor::xz) //
				("mfcompress", usm_compressor::mfcompress);
		}
	} usm_compr_mode;

	const auto aid_est = (x3::lit("-est=") > aid_est_formula) | x3::attr(defaults::aid_formula);
	const auto usm_level = -(x3::lit("-level=") > x3::uint_);

	const auto otherdist_jobspec = x3::rule<struct otherdist_jobspec_class, otherdist_options, true>{} = dist_mode;
	const auto aid_jobspec = x3::rule<struct aid_jobspec_class, aid_options, true>{} = "approxinfo" > aid_est;
	const auto usm_jobspec = x3::lit("usm") > "-est=" > usm_est_formula > "-compr=" > usm_compr_mode > usm_level;

	const auto dist_jobspec_def = otherdist_jobspec | aid_jobspec | usm_jobspec;
	const auto dist_jobspec = x3::rule<struct dist_jobspec_class, dist_options, true>{} = //
		x3::eps > dist_jobspec_def > x3::eoi;

	/*
	 * Error handling
	 */
	const map<string, string> friendly_parser_names = {
		//common
		{typeid(binary_val).name(), "0 or 1"}, //
		{typeid(+binary_val).name(), "0 or 1"}, //
		{typeid(x3::uint_).name(), "a positive integer"}, //
		{typeid(nonnegative_double).name(), "a positive number"},
		//repr-mode
		{typeid(repr_jobspec_def).name(), R"("cgr" or "sparsecgr" or "descr")"},
		/*{typeid(repr_jobspec_def).name(),
			R"("cgr" or "krap" or "cv" or "sparsecgr" or "sparsekrap" or "sparsecv" or "descr")"},*/
		{typeid(cgrlike_k_or_spacedseed).name(), R"("-k=")"},
		/*{typeid(cgrlike_k_or_spacedseed).name(), R"("-k=" or "-spacedseed=")"},*/
		{typeid(cgrlike_order_str).name(), "2 or 4 uppercase letters"},
		{typeid(descr_bins_list).name(), "an increasing, comma-separated list of positive numbers"},
		{typeid(descr_windows_list).name(), "a comma-separated list of positive integers"},
		//dist-mode
		{typeid(dist_jobspec_def).name(),
			R"("euclidean" or "sqeuclidean" or "manhattan" or "cosine" or "pearson" or "approxinfo" or "ssim")"},
		/*{typeid(dist_jobspec_def).name(),
			R"("euclidean" or "sqeuclidean" or "manhattan" or "cosine" or "pearson" or "approxinfo" or "ssim" or "usm")"},*/
		{typeid(aid_est_formula).name(), R"("A" or "B")"},
		{typeid(usm_est_formula).name(), R"("ucd" or "ncd" or "ncd2" or "cd" or "cdm")"},
		{typeid(usm_compr_mode).name(), R"("paq8" or "zpaq" or "gzip" or "bzip2" or "xz" or "mfcompress")"}};
}

struct jobspec_list_separator {
	template <typename InputIterator, typename Token>
	bool operator()(InputIterator &next, InputIterator end, Token &tok) {
		if (next == end) {
			return false;
		}

		tok = Token();
		bool in_brackets = false;
		while (next != end) {
			if (*next == ',' && !in_brackets) {
				++next;
				return true;
			} else if (*next == '[') {
				in_brackets = true;
			} else if (*next == ']') {
				in_brackets = false;
			}

			tok += *next;
			++next;
		}
		return true;
	}

	void reset() {}
};

template <typename JobOptions, typename Parser, typename Func>
vector<JobOptions> parse_jobs(const string &jobspecs, const Parser &parser, const string &usage_str,
	/**/ Func verify_job) {
	vector<JobOptions> jobs;

	for (const string &jobspec : tokenizer<jobspec_list_separator>(jobspecs)) {
		try {
			JobOptions curr_job;

			if (!x3::parse(jobspec.begin(), jobspec.end(), parser, curr_job)) {
				throw runtime_error("Unknown parse error, on: " + jobspec);
			}
			verify_job(curr_job, jobs, jobspec);

			jobs.push_back(curr_job);
		} catch (const invalid_argument &ex) {
			cerr << error << ex.what() << endl << endl;
			cerr << usage::main << endl << usage_str;
			throw already_handled_error();
		} catch (const x3::expectation_failure<std::string::const_iterator> &ex) {
			if (ex.which() == "eoi") {
				cerr << error << "Invalid job specification: unexpected value here: " << endl;
			} else if (parsers::friendly_parser_names.count(ex.which())) {
				cerr << error << "Invalid job specification: expected " << parsers::friendly_parser_names.at(ex.which())
					 << " here: " << endl;
			} else {
				cerr << error << "Invalid job specification: expected " << ex.which() << " here: " << endl;
			}
			cerr << "      " << jobspec << endl;
			cerr << "      " << string(ex.where() - jobspec.begin(), ' ') << '^' << endl << endl;
			cerr << usage::main << endl << usage_str;
			throw already_handled_error();
		}
	}

	if (jobs.empty()) {
		cerr << error << "The option 'job' is required but missing" << endl << endl;
		usage::put_options_help(cerr);
		throw already_handled_error();
	}

	return jobs;
}

vector<repr_options> parse_repr_jobs(const string &jobspecs) {
	return parse_jobs<repr_options>(jobspecs, parsers::repr_jobspec, usage::repr_jobs,
		[](const repr_options &curr_job, const vector<repr_options> &jobs, const string &jobspec) {
			if (!jobs.empty() && jobs[0].type() == typeid(descr_options) && curr_job.type() != typeid(descr_options)) {
				throw invalid_argument("Input type for job type 'descr' conflicts with other given jobs");
			}
			try {
				if (curr_job.type() == typeid(cgrlike_options)) {
					auto curr_cgrlike_job = get<cgrlike_options>(curr_job);
					if (curr_cgrlike_job.k_or_spacedseed.type() == typeid(unsigned) &&
						/**/ get<unsigned>(curr_cgrlike_job.k_or_spacedseed) < 1) {
						throw invalid_argument("Value for parameter 'k' must be at least 1");
					}
				}
				if (curr_job.type() == typeid(sparse_cgrlike_options)) {
					auto curr_sparse_cgrlike_job = get<sparse_cgrlike_options>(curr_job);
					if (curr_sparse_cgrlike_job.k_or_spacedseed.type() == typeid(unsigned) &&
						/**/ get<unsigned>(curr_sparse_cgrlike_job.k_or_spacedseed) < 1) {
						throw invalid_argument("Value for parameter 'k' must be at least 1");
					}
					if (curr_sparse_cgrlike_job.freqcut <= 0) {
						throw invalid_argument("Value for parameter 'freqcut' must be greater than 0");
					}
				}
				if (curr_job.type() == typeid(descr_options)) {
					auto curr_descr_job = get<descr_options>(curr_job);
					if (boost::algorithm::any_of(curr_descr_job.bins, [](double bin) { return bin <= 0; })) {
						throw invalid_argument("Values for parameter 'bins' must be greater than 0");
					}
					if (boost::algorithm::any_of(curr_descr_job.windows, [](unsigned window) { return window < 1; })) {
						throw invalid_argument("Values for parameter 'windows' must be at least 1");
					}
					if (!boost::algorithm::is_sorted(curr_descr_job.bins, std::less<>())) {
						throw invalid_argument(
							"Value for parameter 'bins' must be an increasing list of positive numbers");
					}
				}
			} catch (const invalid_argument &ex) {
				throw invalid_argument(string(ex.what()) + ",\n      in job specification '" + jobspec + "'");
			}
		});
}

vector<dist_options> parse_dist_jobs(const string &jobspecs) {
	return parse_jobs<dist_options>(jobspecs, parsers::dist_jobspec, usage::dist_jobs,
		[](const dist_options &curr_job, const vector<dist_options> &jobs, const string & /*jobspec*/) {
			if (!jobs.empty() && jobs[0].type() == typeid(usm_options) && curr_job.type() != typeid(usm_options)) {
				throw invalid_argument("Input type for job type 'usm' conflicts with other given jobs");
			}
		});
}