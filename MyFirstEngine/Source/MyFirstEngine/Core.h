#pragma once

#ifdef MFE_PLATFORM_WINDOWS

#else
	#error MyFirstEngine only supports Windows!
#endif

#ifdef MFE_DEBUG
	#define MFE_ENABLE_ASSERTS
#endif

#ifdef MFE_ENABLE_ASSERTS
	#define MFE_CORE_ASSERT(x, ...) { if(!(x)) { MFE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define MFE_ASSERT(x, ...) { if(!(x)) { MFE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define MFE_CORE_ASSERT(x, ...)
	#define MFE_ASSERT(x, ...)
#endif
	

#define BIT(x) (1 << x)

#define MFE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace MyFirstEngine 
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}