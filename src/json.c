#include "headers/json.h"
#include <stdio.h>
#include <string.h>

void fprint_json_string(FILE* out, const char* str) {
    if (!str) {
        fprintf(out, "null");
        return;
    }
    
    fputc('"', out);
    while (*str) {
        switch (*str) {
            case '"':  fprintf(out, "\\\""); break;
            case '\\': fprintf(out, "\\\\"); break;
            case '\n': fprintf(out, "\\n"); break;
            case '\r': fprintf(out, "\\r"); break;
            case '\t': fprintf(out, "\\t"); break;
            default:   fputc(*str, out); break;
        }
        str++;
    }
    fputc('"', out);
}

static const char* category_to_string(LicenseCategory cat) {
    switch (cat) {
        case CAT_PERMISSIVE:    return "permissive";
        case CAT_COPYLEFT:      return "copyleft";
        case CAT_PUBLIC_DOMAIN: return "public_domain";
        default:                return "other";
    }
}

void print_json_license(const License* lic, const char* text, FILE* out) {
    fprintf(out, "{\n");
    fprintf(out, "  \"id\": "); fprint_json_string(out, lic->id); fprintf(out, ",\n");
    fprintf(out, "  \"name\": "); fprint_json_string(out, lic->name); fprintf(out, ",\n");
    fprintf(out, "  \"spdx_id\": "); fprint_json_string(out, lic->spdx_id); fprintf(out, ",\n");
    fprintf(out, "  \"category\": \"%s\",\n", category_to_string(lic->category));
    fprintf(out, "  \"text\": "); fprint_json_string(out, text); fprintf(out, "\n");
    fprintf(out, "}\n");
}

void print_json_list(const License* licenses, int count, FILE* out) {
    fprintf(out, "[\n");
    for (int i = 0; i < count; i++) {
        fprintf(out, "  {\n");
        fprintf(out, "    \"id\": "); fprint_json_string(out, licenses[i].id); fprintf(out, ",\n");
        fprintf(out, "    \"name\": "); fprint_json_string(out, licenses[i].name); fprintf(out, ",\n");
        fprintf(out, "    \"spdx_id\": "); fprint_json_string(out, licenses[i].spdx_id); fprintf(out, ",\n");
        fprintf(out, "    \"description\": "); fprint_json_string(out, licenses[i].description); fprintf(out, ",\n");
        fprintf(out, "    \"category\": \"%s\",\n", category_to_string(licenses[i].category));
        fprintf(out, "    \"needs_name\": %s,\n", licenses[i].needs_name ? "true" : "false");
        fprintf(out, "    \"needs_year\": %s\n", licenses[i].needs_year ? "true" : "false");
        fprintf(out, "  }%s\n", (i < count - 1) ? "," : "");
    }
    fprintf(out, "]\n");
}

void print_json_help(const License* lic, 
                     char** permissions, int perm_count,
                     char** conditions, int cond_count,
                     char** limitations, int lim_count,
                     FILE* out) {
    fprintf(out, "{\n");
    fprintf(out, "  \"id\": "); fprint_json_string(out, lic->id); fprintf(out, ",\n");
    fprintf(out, "  \"name\": "); fprint_json_string(out, lic->name); fprintf(out, ",\n");
    fprintf(out, "  \"spdx_id\": "); fprint_json_string(out, lic->spdx_id); fprintf(out, ",\n");
    fprintf(out, "  \"description\": "); fprint_json_string(out, lic->description); fprintf(out, ",\n");
    fprintf(out, "  \"category\": \"%s\",\n", category_to_string(lic->category));
    
    fprintf(out, "  \"permissions\": [");
    for (int i = 0; i < perm_count; i++) {
        fprint_json_string(out, permissions[i]);
        if (i < perm_count - 1) fprintf(out, ", ");
    }
    fprintf(out, "],\n");
    
    fprintf(out, "  \"conditions\": [");
    for (int i = 0; i < cond_count; i++) {
        fprint_json_string(out, conditions[i]);
        if (i < cond_count - 1) fprintf(out, ", ");
    }
    fprintf(out, "],\n");
    
    fprintf(out, "  \"limitations\": [");
    for (int i = 0; i < lim_count; i++) {
        fprint_json_string(out, limitations[i]);
        if (i < lim_count - 1) fprintf(out, ", ");
    }
    fprintf(out, "]\n");
    
    fprintf(out, "}\n");
}
