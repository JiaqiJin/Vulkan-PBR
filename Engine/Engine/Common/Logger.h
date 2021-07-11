#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

#define K_TRACE(...)         ::Log::GetClientLogger()->trace(__VA_ARGS__)
#define K_INFO(...)          ::Log::GetClientLogger()->info(__VA_ARGS__)
#define K_WARN(...)          ::Log::GetClientLogger()->warn(__VA_ARGS__)
#define K_ERROR(...)         ::Log::GetClientLogger()->error(__VA_ARGS__)
#define K_CRITICAL(...)      ::Log::GetClientLogger()->critical(__VA_ARGS__)