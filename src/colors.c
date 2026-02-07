#include "headers/colors.h"
#include <unistd.h>
#include <stdio.h>

static bool g_colors_enabled = true;

void colors_init(void) {
    /* Disable colors if stdout is not a TTY */
    if (!isatty(STDOUT_FILENO)) {
        g_colors_enabled = false;
    }
}

void colors_disable(void) {
    g_colors_enabled = false;
}

bool colors_enabled(void) {
    return g_colors_enabled;
}

const char* col(const char* color_code) {
    return g_colors_enabled ? color_code : "";
}

void print_colored(const char* color, const char* text) {
    if (g_colors_enabled) {
        printf("%s%s%s", color, text, COL_RESET);
    } else {
        printf("%s", text);
    }
}
