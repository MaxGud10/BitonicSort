#pragma once

#if defined __has_include
#if __has_include(<format>)

#include <format>

#ifdef BS_LOG

#define MSG(msg)                                                               \
                                                                               \
    do                                                                         \
    {                                                                          \
        std::clog << __FUNCTION__ << ": ";                                     \
        std::clog << msg;                                                      \
    }                                                                          \
    while (false)

#define LOG(msg, ...)                                                          \
    do                                                                         \
    {                                                                          \
        std::clog << __FUNCTION__ << ": ";                                     \
        std::clog << std::format(msg, __VA_ARGS__);                            \
    }                                                                          \
    while (false)

#ifdef BS_DUMP
  #define BS_STEP_DUMP(state, label, before, after, stage, pass, a, b, swapped) \
      ::bitonic::dump::step_dump(state, __FILE__, __LINE__, __func__, label, before, after, stage, pass, a, b, swapped)
#else
  #define BS_STEP_DUMP(state, label, before, after, stage, pass, a, b, swapped) \
      do{}while(0)
#endif

#else

#define MSG(msg)                                                               \
    do                                                                         \
    {                                                                          \
    }                                                                          \
    while (false)
#define LOG(msg, ...)                                                          \
    do                                                                         \
    {                                                                          \
    }                                                                          \
    while (false)

#endif // BS_LOG

#else

#define MSG(msg)                                                               \
    do                                                                         \
    {                                                                          \
    }                                                                          \
    while (false)
#define LOG(msg, ...)                                                          \
    do                                                                         \
    {                                                                          \
    }                                                                          \
    while (false)

#endif // __has_include(<format>)
#endif // defined __has_include