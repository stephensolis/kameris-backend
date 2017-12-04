#ifndef _LIBMMG_UTILS_PARALLEL_
#define _LIBMMG_UTILS_PARALLEL_

#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

namespace mmg {
	class ParallelExecutor {
	 private:
		template <typename Func>
		using func_result = typename std::result_of<Func(unsigned)>::type;

		std::vector<std::thread> workers;

		std::vector<std::function<void(unsigned)>> tasks;
		std::atomic<size_t> curr_task = {0};
		bool started = false;

	 public:
		ParallelExecutor() = default;

		ParallelExecutor(const ParallelExecutor &other) {
			if (started) {
				throw std::logic_error("Copying is not possible once execution has started");
			}

			tasks = other.tasks;
		}

		ParallelExecutor &operator=(const ParallelExecutor &other) {
			if (started) {
				throw std::logic_error("Copying is not possible once execution has started");
			}

			tasks = other.tasks;
			return *this;
		}

		//it's not really possible to write noexcept moves for this class
		ParallelExecutor(ParallelExecutor &&other) = delete;
		ParallelExecutor &operator=(ParallelExecutor &&other) = delete;

		template <typename Func>
		auto enqueue(Func func) -> std::future<func_result<Func>> {
			if (started) {
				throw std::logic_error("Adding more tasks is not possible once execution has started");
			}

			auto task = std::make_shared<std::packaged_task<func_result<Func>(unsigned)>>(func);
			tasks.emplace_back([task](unsigned thread_id) {
				(*task)(thread_id); //
			});

			return task->get_future();
		}

		void execute(unsigned threads) {
			if (started) {
				throw std::logic_error("Execution can only be started once");
			}
			started = true;

			for (unsigned i = 0; i < threads; ++i) {
				workers.emplace_back([i, this]() {
					while (true) {
						size_t curr_task = this->curr_task.fetch_add(1, std::memory_order_relaxed);

						if (curr_task >= this->tasks.size()) {
							return;
						}

						this->tasks[curr_task](i);
					}
				});
			}
		}

		~ParallelExecutor() {
			for (std::thread &worker : workers) {
				worker.join();
			}
		}
	};
}

#endif
