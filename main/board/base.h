#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define MAX_ERRORS 10

typedef enum {
    ERROR_NONE =0,
    ERROR_CANNOT_READ_RTC,
    ERROR_CANNOT_READ_LASER_SENSOR,
    ERROR_WIFI_LOSS_CONNECT,
    ERROR_WIFI_CONNECT_TIMEOUT,
    ERROR_DATE_TAKE_FROM_SERVER_WRONG_FORMAT
} ErrorCode;
typedef void (*error_call_back) (void*);
typedef struct {
    ErrorCode code;
    char message[100];
} Error;

typedef struct {
    Error errors[MAX_ERRORS];
    int count;
    error_call_back call_back;
} ErrorList;



void add_error(void *arg, ErrorCode code, const char *message);
const char* get_error_string(void *arg, const Error *error);
bool has_error(const ErrorList *arg, ErrorCode code);
bool clear_error(ErrorList *list, int code);
extern ErrorList error_list;
extern void signal_new_error(void * arg);
#endif // BASE_H
