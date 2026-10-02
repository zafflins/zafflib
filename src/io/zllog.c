#include <zl/zllog.h>

static struct LogInternal {
    char* info;
    char* warn;
    char* error;
    char* fatal;
} LogInternal = {0};

void Log(i32 level, char* message) {
    if (!message || level > ZL_LOG_FATAL) return;
    switch(level) {
        default: return;
        case (ZL_LOG_INFO): {
            LogInternal.info = message;
        } return;
        case (ZL_LOG_WARN): {
            LogInternal.warn = message;
        } return;
        case (ZL_LOG_ERROR): {
            LogInternal.error = message;
        } return;
        case (ZL_LOG_FATAL): {
            LogInternal.fatal = message;
        } return;
    }
}

const char* GetLog(i32 level) {
    if (level > ZL_LOG_FATAL) return NULL;
    switch(level) {
        default: return NULL;
        case (ZL_LOG_INFO): return LogInternal.info;
        case (ZL_LOG_WARN): return LogInternal.warn;
        case (ZL_LOG_ERROR): return LogInternal.error;
        case (ZL_LOG_FATAL): return LogInternal.fatal;
    }
}
