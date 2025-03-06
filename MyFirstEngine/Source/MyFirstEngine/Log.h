#pragma once

#include "Core.h"
#include "spdlog\spdlog.h"
#include "spdlog\fmt\ostr.h"

namespace MyFirstEngine
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_ClientLogger;
	};
}

// Core log macros
#define MFE_CORE_TRACE(...) ::MyFirstEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define MFE_CORE_INFO(...) ::MyFirstEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define MFE_CORE_WARN(...) ::MyFirstEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MFE_CORE_ERROR(...) ::MyFirstEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MFE_CORE_CRITICAL(...) ::MyFirstEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Cliend log macros
#define MFE_TRACE(...) ::MyFirstEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define MFE_INFO(...) ::MyFirstEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define MFE_WARN(...) ::MyFirstEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MFE_ERROR(...) ::MyFirstEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define MFE_CRITICAL(...) ::MyFirstEngine::Log::GetClientLogger()->critical(__VA_ARGS__)