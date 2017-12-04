#ifndef _CLI_COLORS_
#define _CLI_COLORS_

#include <cstdio>
#include <iostream>

#ifdef _WIN32
#include <io.h>
#include <windows.h>

namespace colors_impl {
	inline WORD get_color_attrs() {
		CONSOLE_SCREEN_BUFFER_INFO buffer_info{};
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buffer_info);
		return buffer_info.wAttributes;
	}

	static const WORD default_color_attrs = get_color_attrs();
	static const WORD default_background_attrs =
		default_color_attrs & (BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
}

#define DEF_COLOR(name, ansi_code, windows_code)                                 \
	inline std::ostream &name(std::ostream &stream) { /* NOLINT */               \
		WORD real_code = (windows_code) | colors_impl::default_background_attrs; \
		if (&stream == &std::cout && _isatty(_fileno(stdout))) {                 \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), real_code); \
		} else if (&stream == &std::cerr && _isatty(_fileno(stderr))) {          \
			SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), real_code);  \
		}                                                                        \
		return stream;                                                           \
	}
#else
#include <unistd.h>

#define DEF_COLOR(name, ansi_code, windows_code)                                                                      \
	inline std::ostream &name(std::ostream &stream) { /* NOLINT */                                                    \
		if ((&stream == &std::cout && isatty(fileno(stdout))) || (&stream == &std::cerr && isatty(fileno(stderr)))) { \
			stream << "\033[" #ansi_code;                                                                             \
		}                                                                                                             \
		return stream;                                                                                                \
	}
#endif

namespace colors {
	DEF_COLOR(grey, 30m, 0)
	DEF_COLOR(red, 31m, FOREGROUND_RED)
	DEF_COLOR(green, 32m, FOREGROUND_GREEN)
	DEF_COLOR(yellow, 33m, FOREGROUND_GREEN | FOREGROUND_RED)
	DEF_COLOR(blue, 34m, FOREGROUND_BLUE)
	DEF_COLOR(magenta, 35m, FOREGROUND_BLUE | FOREGROUND_RED)
	DEF_COLOR(cyan, 36m, FOREGROUND_BLUE | FOREGROUND_GREEN)
	DEF_COLOR(white, 37m, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED)

	DEF_COLOR(default_color, 39m, colors_impl::default_color_attrs)
}

#endif
