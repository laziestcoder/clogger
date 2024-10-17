#include "logger.h"
#include <assert.h>
#include <stdio.h>

void test_logger_sync() {
    Logger logger;
    init_logger(&logger, LOG_INFO, 0); // Synchronous logger

    log_message(&logger, LOG_DEBUG, "This should not be logged.");
    log_message(&logger, LOG_INFO, "Info: Synchronous logger test.");
    log_message(&logger, LOG_WARN, "Warning: Synchronous logger test.");
    
    set_log_level(&logger, LOG_DEBUG);
    log_message(&logger, LOG_DEBUG, "Debug: This should now be logged.");

    stop_logger(&logger);
}

void test_logger_async() {
    Logger logger;
    init_logger(&logger, LOG_INFO, 1); // Asynchronous logger

    log_message(&logger, LOG_DEBUG, "This should not be logged.");
    log_message(&logger, LOG_INFO, "Info: Asynchronous logger test.");
    log_message(&logger, LOG_WARN, "Warning: Asynchronous logger test.");
    
    set_log_level(&logger, LOG_DEBUG);
    log_message(&logger, LOG_DEBUG, "Debug: This should now be logged.");

    stop_logger(&logger);
}

int main() {
    printf("Testing synchronous logger...\n");
    test_logger_sync();

    printf("Testing asynchronous logger...\n");
    test_logger_async();

    printf("All tests passed!\n");
    return 0;
}

