#include "base.h"
#include "esp_log.h"
static const char *TAG = "ErrorList";

ErrorList error_list = { 
    .count = 0, 
    .errors = { { .code = ERROR_NONE, .message = "No error" } },
    .call_back = signal_new_error
};

void add_error(void *arg, ErrorCode code, const char *message) {
    ErrorList* list = (ErrorList*) arg;
    if (list->count >= MAX_ERRORS){
        ESP_LOGI(TAG, "Error list is full");
        return;
    }
    else if (list->errors[list->count-1].code == code)
    {
        ESP_LOGI(TAG, "same error, pass add error");
        return;
    }
    list->errors[list->count].code = code;
    strncpy(list->errors[list->count].message, message, sizeof(list->errors[list->count].message) - 1);
    list->errors[list->count].message[sizeof(list->errors[list->count].message) - 1] = '\0';
    list->count++;
    if(!list->call_back) list->call_back(&error_list);
}
const char* get_error_string(void* arg, const Error *error) {
    ErrorList* error_list  = (ErrorList*)arg;
    static char buffer[150];
    snprintf(buffer, sizeof(buffer), "<%d>:<%s>", error->code, error->message);
    return buffer;
}

bool has_error(const ErrorList *list, ErrorCode code) {
    for (int i = 0; i < list->count; i++) {
        if (list->errors[i].code == code) {
            ESP_LOGI(TAG, "Error found: %s", get_error_string(&error_list, &list->errors[i]));
            return true;
        }
    }
    return false;
}

bool clear_error(ErrorList *list, int code) {
    if (code == -1) {
        list->count = 0;
        return true;
    } else {
        ErrorCode errorCode = (ErrorCode)code;
        for (int i = 0; i < list->count; i++) {
            if (list->errors[i].code == errorCode) {
                for (int j = i; j < list->count - 1; j++) {
                    list->errors[j] = list->errors[j + 1];
                }
                list->count--;
                return true;
            }
        }
    }
    return false;
}
