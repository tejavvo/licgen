#ifndef LICGEN_H
#define LICGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define LICGEN_VERSION "1.0.0"
#define LICGEN_NAME "licgen"

#define MAX_LICENSE_TEXT 65536
#define MAX_NAME_LENGTH 256
#define MAX_YEAR_LENGTH 8

/* License categories */
typedef enum {
    CAT_PERMISSIVE,
    CAT_COPYLEFT,
    CAT_PUBLIC_DOMAIN,
    CAT_OTHER
} LicenseCategory;

/* License structure */
typedef struct {
    char* id;
    char* name;
    char* spdx_id;
    char* description;
    LicenseCategory category;
    bool needs_name;
    bool needs_year;
    
    char** permissions;
    int num_permissions;
    char** conditions;
    int num_conditions;
    char** limitations;
    int num_limitations;
    
    char* template_text;
} License;

/* License Registry */
const License* get_license_by_id(const char* id);
const License* get_all_licenses(int* count);
void load_all_templates(const char* directory);
void free_licenses(void);
void print_license_list(bool permissive, bool copyleft, bool public_domain);

/* String utilities */
static inline char* safe_strdup(const char* s) {
    if (!s) return NULL;
    char* dup = malloc(strlen(s) + 1);
    if (dup) strcpy(dup, s);
    return dup;
}

#endif /* LICGEN_H */
