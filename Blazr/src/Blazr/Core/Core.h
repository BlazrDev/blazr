#pragma once

#ifdef BLZR_PLATFORM_WINDOWS
#ifdef BLZR_BUILD_DLL
#define BLZR_API __declspec(dllexport)
#else
#define BLZR_API __declspec(dllimport)
#endif
#elif BLZR_PLATFORM_LINUX
#define BLZR_API __attribute__((visibility("default")))
#else
#error Blazer only supports Linux and Windows
#endif

#define BIT(x) (1 << x)
