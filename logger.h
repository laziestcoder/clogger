#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <pthread.h>

// Log levels
typedef enum {
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG
} LogLevel;

// Logger structure
typedef struct Logger {
    LogLevel level;
    pthread_mutex_t mutex;
    pthread_t log_thread;
    int async; // Mode switch: 1 for async, 0 for sync
} Logger;

// Logger functions
void init_logger(Logger *logger, LogLevel level, int async);
void set_log_level(Logger *logger, LogLevel level);
void log_message(Logger *logger, LogLevel level, const char *format, ...);
void stop_logger(Logger *logger);

// Internal worker function for async logging
void *logger_worker(void *arg);

#endif

