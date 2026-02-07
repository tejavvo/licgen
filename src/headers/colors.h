#ifndef COLORS_H
#define COLORS_H

#include <stdbool.h>

/* ANSI color codes */
#define COL_RESET   "\033[0m"
#define COL_BOLD    "\033[1m"
#define COL_DIM     "\033[2m"

#define COL_RED     "\033[31m"
#define COL_GREEN   "\033[32m"
#define COL_YELLOW  "\033[33m"
#define COL_BLUE    "\033[34m"
#define COL_MAGENTA "\033[35m"
#define COL_CYAN    "\033[36m"
#define COL_WHITE   "\033[37m"

#define COL_BRED    "\033[1;31m"
#define COL_BGREEN  "\033[1;32m"
#define COL_BYELLOW "\033[1;33m"
#define COL_BBLUE   "\033[1;34m"
#define COL_BMAGENTA "\033[1;35m"
#define COL_BCYAN   "\033[1;36m"
#define COL_BWHITE  "\033[1;37m"

/* Color control functions */
void colors_init(void);
void colors_disable(void);
bool colors_enabled(void);

/* Color-aware printing */
const char* col(const char* color_code);
void print_colored(const char* color, const char* text);

#endif /* COLORS_H */
