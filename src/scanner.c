// Aaron Sprouse 2024

#include "scanner.h"

int scan_callback([[maybe_unused]] YR_SCAN_CONTEXT *context, int message,
                  void *message_data, void *user_data) {
    switch (message) {
    case CALLBACK_MSG_RULE_MATCHING:
        printf("Rule match for rule: %s \nFile: %s\n",
               ((YR_RULE *)message_data)->identifier,
               ((filedata_t *)user_data)->filename);
        return CALLBACK_CONTINUE;
    default:
        return CALLBACK_CONTINUE;
    }
}