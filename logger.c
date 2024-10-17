#include "logger.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 1024

typedef struct LogQueue {
    char messages[BUFFER_SIZE][256];
    int head;
    int tail;
    int size;
    pthread_cond_t cond;
    pthread_mutex_t queue_mutex;
} LogQueue;

LogQueue queue;

void enqueue_log(const char *message) {
    pthread_mutex_lock(&queue.queue_mutex);
    if (queue.size < BUFFER_SIZE) {
        strcpy(queue.messages[queue.tail], message);
        queue.tail = (queue.tail + 1) % BUFFER_SIZE;
        queue.size++;
        pthread_cond_signal(&queue.cond);
    }
    pthread_mutex_unlock(&queue.queue_mutex);
}

char* dequeue_log() {
    pthread_mutex_lock(&queue.queue_mutex);
    while (queue.size == 0) {
        pthread_cond_wait(&queue.cond, &queue.queue_mutex);
    }
    char *message = strdup(queue.messages[queue.head]);
    queue.head = (queue.head + 1) % BUFFER_SIZE;
    queue.size--;
    pthread_mutex_unlock(&queue.queue_mutex);
    return message;
}

void *logger_worker(void *arg) {
    Logger *logger = (Logger *)arg;
    while (1) {
        char *message = dequeue_log();
        printf("%s\n", message);
        free(message);
    }
    return NULL;
}

void init_logger(Logger *logger, LogLevel level, int async) {
    logger->level = level;
    logger->async = async;
    pthread_mutex_init(&logger->mutex, NULL);

    queue.head = 0;
    queue.tail = 0;
    queue.size = 0;
    pthread_cond_init(&queue.cond, NULL);
    pthread_mutex_init(&queue.queue_mutex, NULL);

    if (async) {
        pthread_create(&logger->log_thread, NULL, logger_worker, logger);
    }
}

void set_log_level(Logger *logger, LogLevel level) {
    pthread_mutex_lock(&logger->mutex);
    logger->level = level;
    pthread_mutex_unlock(&logger->mutex);
}

void log_message(Logger *logger, LogLevel level, const char *format, ...) {
    if (level > logger->level) {
        return;
    }

    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (logger->async) {
        enqueue_log(buffer);
    } else {
        pthread_mutex_lock(&logger->mutex);
        printf("%s\n", buffer);
        pthread_mutex_unlock(&logger->mutex);
    }
}

void stop_logger(Logger *logger) {
    if (logger->async) {
        pthread_cancel(logger->log_thread);
        pthread_join(logger->log_thread, NULL);
    }
    pthread_mutex_destroy(&logger->mutex);
}

