#ifndef _LIBKAMERIS_UTILS_PARALLEL_
#define _LIBKAMERIS_UTILS_PARALLEL_

#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

namespace kameris {
	class ParallelExecutor {
	 private:
		template <typename Func>
		using func_result = typename std::result_of<Func(unsigned)>::type;

		std::vector<std::thread> _workers;

		std::vector<std::function<void(unsigned)>> _tasks;
		std::atomic<size_t> _curr_task = {0};
		bool _started = false;

	 public:
		ParallelExecutor() = default;

		ParallelExecutor(const ParallelExecutor &other) = delete;
		ParallelExecutor &operator=(const ParallelExecutor &other) = delete;
		ParallelExecutor(ParallelExecutor &&other) = delete;
		ParallelExecutor &operator=(ParallelExecutor &&other) = delete;

		template <typename Func>
		auto enqueue(Func func) -> std::future<func_result<Func>> {
			if (_started) {
				throw std::logic_error("Adding more tasks is not possible once execution has started");
			}

			auto task = std::make_shared<std::packaged_task<func_result<Func>(unsigned)>>(func);
			_tasks.emplace_back([task](unsigned thread_id) {
				(*task)(thread_id); //
			});

			return task->get_future();
		}

		void execute(unsigned threads) {
			if (threads == 0) {
				throw std::invalid_argument("At least one thread must be started");
			}

			if (_started) {
				throw std::logic_error("Execution can only be started once");
			}
			_started = true;

			for (unsigned thread_id = 0; thread_id < threads; ++thread_id) {
				_workers.emplace_back([thread_id, this]() {
					while (true) {
						size_t fetched_task = _curr_task.fetch_add(1, std::memory_order_relaxed);

						if (fetched_task >= _tasks.size()) {
							return;
						}

						_tasks[fetched_task](thread_id);
					}
				});
			}
		}

		~ParallelExecutor() {
			for (std::thread &worker : _workers) {
				worker.join();
			}
		}
	};
}

#endif
