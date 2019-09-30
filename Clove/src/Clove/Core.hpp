#pragma once

//Utility defines
#if CLV_DEBUG || CLV_DEVELOPMENT
	#define CLV_ENABLE_ASSERTS 1
#else
	#define CLV_ENABLE_ASSERTS 0
#endif

#if CLV_PROFILING
	#define CLV_ENABLE_PROFILING 1
#else
	#define CLV_ENABLE_PROFILING 0
#endif

#if CLV_ENABLE_ASSERTS
	#define CLV_ASSERT(x, ...) { if(!(x)){ CLV_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); CLV_DEBUG_BREAK; } }
#else
	#define CLV_ASSERT(x, ...) (x)
#endif

#define BIT(x) (1 << x)

#if defined(__clang__)
	#define CLV_DEBUG_BREAK __builtin_debugtrap()
#elif (defined(__GNUC__) || defined(__GNUG__))
	#define CLV_DEBUG_BREAK __builtin_trap()
#elif defined(_MSC_VER)
	#define CLV_DEBUG_BREAK __debugbreak()
#endif

#if defined(_MSC_VER)
	#define	CLV_FUNCTION_NAME __FUNCTION__
#else 
	#define	CLV_FUNCTION_NAME __func__
#endif

#if CLV_PLATFORM_WINDOWS
	#define CLV_APIENTRY APIENTRY
#else
	#define CLV_APIENTRY
#endif
