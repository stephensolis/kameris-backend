#ifndef _COMMON_MEMORY_USAGE_
#define _COMMON_MEMORY_USAGE_

#include <cstdint>

#ifdef _WIN32
#include <windows.h>

inline uint64_t total_free_memory_bytes() {
	MEMORYSTATUSEX status{};
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullAvailPhys;
}
#elif defined(BSD) || defined(__MACH__) || defined(__QNX__)
#include <sys/sysctl.h>
#include <sys/types.h>

inline uint64_t total_free_memory_bytes() {
	uint64_t page_size;
	size_t len = sizeof(page_size);
	sysctlbyname("hw.pagesize", &page_size, &len, nullptr, 0);

	uint64_t free_pages;
	len = sizeof(free_pages);
	sysctlbyname("vm.stats.vm.v_free_count", &free_pages, &len, nullptr, 0);

	return page_size * free_pages;
}
#else
#include <unistd.h>

inline uint64_t total_free_memory_bytes() {
	return uint64_t(sysconf(_SC_AVPHYS_PAGES)) * sysconf(_SC_PAGESIZE);
}
#endif

#endif
