#ifndef HELP_H
#define HELP_H

#include "licgen.h"

/* Print general usage help */
void print_usage(void);

/* Print version info */
void print_version(void);

/* License-specific help */
void print_license_help(const License* lic);

/* Helpers for tables */
void print_permissions(char** items, int count);
void print_conditions(char** items, int count);
void print_limitations(char** items, int count);

/* Print a single help item with symbol */
void print_permission_item(const char* item);
void print_condition_item(const char* item);
void print_limitation_item(const char* item);

#endif /* HELP_H */
