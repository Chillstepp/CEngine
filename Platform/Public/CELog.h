//
// Created by Chillstep on 2025/2/3.
//

#ifndef CENGINE_PLATFORM_PUBLIC_CELOG_H_
#define CENGINE_PLATFORM_PUBLIC_CELOG_H_


#include "CEngine.h"
#include "spdlog/common.h"

namespace CE{
	class CELog{
	 public:
		CELog(const CELog&) = delete;
		CELog &operator=(const CELog&) = delete;

		static void Init();

		static CELog& GetLoggerInstance(){
			static CELog sLoggerInstance;
			return sLoggerInstance;
		}

		template<typename... Args>
		void Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args &&...args){
			spdlog::memory_buf_t buf;
			fmt::vformat_to(fmt::appender(buf), fmt, fmt::make_format_args(args...));
			Log(loc, lvl, buf);
		}

	 private:
		CELog() = default;

		void Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, const spdlog::memory_buf_t &buffer);

	};

#define CE_LOG_LOGGER_CALL(CELogInstance, level, ...)\
        (CELogInstance).Log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)

#define LOG_T(...) CE_LOG_LOGGER_CALL(CE::CELog::GetLoggerInstance(), spdlog::level::trace, __VA_ARGS__)
#define LOG_D(...) CE_LOG_LOGGER_CALL(CE::CELog::GetLoggerInstance(), spdlog::level::debug, __VA_ARGS__)
#define LOG_I(...) CE_LOG_LOGGER_CALL(CE::CELog::GetLoggerInstance(), spdlog::level::info, __VA_ARGS__)
#define LOG_W(...) CE_LOG_LOGGER_CALL(CE::CELog::GetLoggerInstance(), spdlog::level::warn, __VA_ARGS__)
#define LOG_E(...) CE_LOG_LOGGER_CALL(CE::CELog::GetLoggerInstance(), spdlog::level::err, __VA_ARGS__)
}

#endif //CENGINE_PLATFORM_PUBLIC_CELOG_H_
