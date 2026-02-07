#include "headers/prompt.h"
#include "headers/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_INPUT 256

char* prompt_string(const char* prompt, const char* default_value) {
    char buffer[MAX_INPUT];
    
    if (default_value && *default_value) {
        printf("%s%s%s [%s%s%s]: ", 
               col(COL_BCYAN), prompt, col(COL_RESET),
               col(COL_DIM), default_value, col(COL_RESET));
    } else {
        printf("%s%s%s: ", col(COL_BCYAN), prompt, col(COL_RESET));
    }
    
    fflush(stdout);
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        if (default_value) {
            char* result = malloc(strlen(default_value) + 1);
            if (result) strcpy(result, default_value);
            return result;
        }
        return NULL;
    }
    
    /* Remove trailing newline */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        len--;
    }
    
    /* Use default if empty */
    if (len == 0 && default_value) {
        char* result = malloc(strlen(default_value) + 1);
        if (result) strcpy(result, default_value);
        return result;
    }
    
    char* result = malloc(len + 1);
    if (result) strcpy(result, buffer);
    return result;
}

char* get_current_year(void) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char* year = malloc(16);
    if (year) {
        snprintf(year, 16, "%d", tm_info->tm_year + 1900);
    }
    return year;
}

char* prompt_year(void) {
    char* current_year = get_current_year();
    char* result = prompt_string("Year", current_year);
    free(current_year);
    return result;
}
