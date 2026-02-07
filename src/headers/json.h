#ifndef JSON_H
#define JSON_H

#include "licgen.h"
#include <stdio.h>

/* Print license as JSON */
void print_json_license(const License* lic, const char* text, FILE* out);

/* Print license list as JSON */
void print_json_list(const License* licenses, int count, FILE* out);

/* Print license help as JSON */
void print_json_help(const License* lic, 
                     char** permissions, int perm_count,
                     char** conditions, int cond_count,
                     char** limitations, int lim_count,
                     FILE* out);

/* JSON string escape utility */
void fprint_json_string(FILE* out, const char* str);

#endif /* JSON_H */
