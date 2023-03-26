#pragma once

//#define PROFILER_DISABLED 
//use this define to turn off profiler without deleting or commenting LOG_DURATION macroes and #include

#ifndef PROFILER_DISABLED

#include "Logging/LogMacros.h"
#include "Containers/UnrealString.h"

#include <chrono>

enum class LogDurationChronoUnit
{
    seconds,
    milliseconds,
    microseconds,
    nanoseconds
};

//Unit is unit of measuremenet, i.e seconds, milliseconds, nanoseconds
class LogDuration {
public:
    LogDuration(LogDurationChronoUnit unit, const FString& msg = "") : unit(unit), message(msg + ": "), start(std::chrono::high_resolution_clock::now())
    {
    }

    ~LogDuration() {
        auto finish = std::chrono::high_resolution_clock::now();
        auto dur = finish - start;

        switch(unit)
        {
        case LogDurationChronoUnit::seconds:
            UE_LOG(LogTemp, Warning, TEXT("%s %lld s"), *message, std::chrono::duration_cast<std::chrono::seconds>(dur).count());
            break;
        case LogDurationChronoUnit::milliseconds:
            UE_LOG(LogTemp, Warning, TEXT("%s %lld ms"), *message, std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());
            break;
        case LogDurationChronoUnit::microseconds:
            UE_LOG(LogTemp, Warning, TEXT("%s %lld mcs"), *message, std::chrono::duration_cast<std::chrono::microseconds>(dur).count());
            break;
        case LogDurationChronoUnit::nanoseconds:
            UE_LOG(LogTemp, Warning, TEXT("%s %lld ns"), *message, std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count());
            break;
        };
    }
private:
    LogDurationChronoUnit unit;
    FString message;
    std::chrono::high_resolution_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(unit, message) \
  LogDuration UNIQ_ID(__LINE__){unit, message};

#else
#define LOG_DURATION(unit, message)

#endif //PROFILER_DISABLED