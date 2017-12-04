#ifndef _COMMON_VERSION_
#define _COMMON_VERSION_

#ifndef VERSION_SHORT
#define VERSION_SHORT "unknown-version"
#endif
#ifndef BUILD_TYPE
#define BUILD_TYPE "unknown-build"
#endif

#define VERSION_LONG VERSION_SHORT " (" BUILD_TYPE "), built " __DATE__

#endif
