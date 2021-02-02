#ifndef DEBUG_TOOLS
#define DEBUG_TOOLS

// TODO: implement levels.

#include <stdio.h>
#include <stdint.h>

#define LOG_LVL_NONE    0
#define LOG_LVL_DEBUG   1
#define LOG_LVL_TRACE   2
#define LOG_LVL_MSG     3
#define LOG_LVL_WARN    4
#define LOG_LVL_ERROR   5
#define LOG_LVL_OUTPUT  6

#define LOG_DEBUG_MSGS
#define LOG_TRACE_MSGS
#define LOG_MSG_MSGS
#define LOG_WARN_MSGS
#define LOG_OUTPUT_MSGS
#define LOG_ERROR_MSGS  

#undef LOG_TRACE_MSGS

#ifdef LOG_OUTPUT_MSGS
    #define LOG_OUTPUT(...) printf("[OUTPUT]: "); \
        printf(__VA_ARGS__)
    #define LOG_OUTPUT_RAW(...) printf(__VA_ARGS__)
#else
    #define LOG_OUTPUT(...)
#endif

#ifdef LOG_ERROR_MSGS
    #define LOG_ERROR(...) printf("[ERROR]: "); \
        printf(__VA_ARGS__)
#else
    #define LOG_ERROR(...)
#endif

#ifdef LOG_WARN_MSGS
    #define LOG_WARN(...) printf("[Warning]: "); \
        printf(__VA_ARGS__)
#else
    #define LOG_WARN(...)
#endif
   
#ifdef LOG_MSG_MSGS
    #define LOG_MSG(...) printf("[Message]: "); \
        printf(__VA_ARGS__)
#else 
    #define LOG_MSG(...)
#endif
   
   
#ifdef LOG_TRACE_MSGS
    #define LOG_TRACE(...) printf("\n[Trace]: "); \
        printf(__VA_ARGS__); \
        printf("\n")
#else
    #define LOG_TRACE(...)
#endif


#ifdef LOG_DEBUG_MSGS
    #define LOG_DEBUG(...) printf("[Debug]: "); \
        printf(__VA_ARGS__)
#else
    #define LOG_DEBUG(...)
#endif

#define LOG(lvl, ...) \
    switch (lvl) \
    { \
    case LOG_LVL_OUTPUT: \
        LOG_OUTPUT(__VA_ARGS__); \
        break; \
    case LOG_LVL_ERROR: \
        LOG_ERROR(__VA_ARGS__); \
        break; \
    case LOG_LVL_WARN: \
        LOG_OUTPUT(__VA_ARGS__); \
        break; \
    case LOG_LVL_MSG: \
        LOG_MSG(__VA_ARGS__); \
        break; \
    case LOG_LVL_TRACE: \
        LOG_TRACE(__VA_ARGS__); \
        break; \
    case LOG_LVL_DEBUG: \
        LOG_DEBUG(__VA_ARGS__); \
        break; \
    }

#endif
