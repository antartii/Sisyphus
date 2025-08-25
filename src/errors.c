#include "errors.h"

const char *SSP_ERROR_STR[] = {
    #define X(code, str, severity) str,
    SSP_ERRORS(X)
    #undef X
};

enum SSP_ERROR_SEVERITY SSPget_error_severity(enum SSP_ERROR_CODE code)
{
    switch (code) {
        #define X(code, str, severity) case code: return severity;
        SSP_ERRORS(X)
        #undef X
        default: return SSP_ERROR_SEVERITY_INFO;
    }
}
