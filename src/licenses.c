#include "headers/licgen.h"
#include "headers/colors.h"
#include "headers/help.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

static License* registry = NULL;
static int license_count = 0;
static int registry_capacity = 10;

static char* trim(char* str) {
    if (!str) return NULL;
    char* start = str;
    while(isspace((unsigned char)*start)) start++;
    
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    if (*str == 0) return str;
    
    char* end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

static char* safe_strdup_trim(const char* s) {
    if (!s) return NULL;
    char* copy = strdup(s);
    return trim(copy);
}

static void add_to_list(char*** list, int* count, const char* item) {
    *list = realloc(*list, (*count + 1) * sizeof(char*));
    (*list)[*count] = safe_strdup_trim(item);
    (*count)++;
}

static void free_license_data(License* lic) {
    free(lic->id);
    free(lic->name);
    free(lic->spdx_id);
    free(lic->description);
    for (int i = 0; i < lic->num_permissions; i++) free(lic->permissions[i]);
    free(lic->permissions);
    for (int i = 0; i < lic->num_conditions; i++) free(lic->conditions[i]);
    free(lic->conditions);
    for (int i = 0; i < lic->num_limitations; i++) free(lic->limitations[i]);
    free(lic->limitations);
    free(lic->template_text);
}

void free_licenses() {
    for (int i = 0; i < license_count; i++) {
        free_license_data(&registry[i]);
    }
    free(registry);
    registry = NULL;
    license_count = 0;
}

static void parse_template_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return;

    License lic = {0};
    lic.needs_name = true;
    lic.needs_year = true;
    
    char line[2048];
    int section = 0; // 0: metadata, 1: permissions, 2: conditions, 3: limitations, 4: text
    
    char* full_text = malloc(MAX_LICENSE_TEXT);
    full_text[0] = '\0';

    while (fgets(line, sizeof(line), f)) {
        if (section < 4 && strncmp(line, "---", 3) == 0) {
            section = 4;
            continue;
        }

        if (section == 4) {
            strcat(full_text, line);
            continue;
        }

        char* trimmed = trim(line);
        if (strlen(trimmed) == 0) continue;

        if (section == 0) {
            if (strncmp(trimmed, "ID:", 3) == 0) lic.id = safe_strdup_trim(trimmed + 3);
            else if (strncmp(trimmed, "Name:", 5) == 0) lic.name = safe_strdup_trim(trimmed + 5);
            else if (strncmp(trimmed, "SPDX:", 5) == 0) lic.spdx_id = safe_strdup_trim(trimmed + 5);
            else if (strncmp(trimmed, "Category:", 9) == 0) {
                char* cat = safe_strdup_trim(trimmed + 9);
                if (strcasecmp(cat, "permissive") == 0) lic.category = CAT_PERMISSIVE;
                else if (strcasecmp(cat, "copyleft") == 0) lic.category = CAT_COPYLEFT;
                else if (strcasecmp(cat, "public") == 0) lic.category = CAT_PUBLIC_DOMAIN;
                else lic.category = CAT_OTHER;
                free(cat);
            }
            else if (strncmp(trimmed, "Description:", 12) == 0) lic.description = safe_strdup_trim(trimmed + 12);
            else if (strncmp(trimmed, "Permissions:", 12) == 0) section = 1;
            else if (strncmp(trimmed, "Conditions:", 11) == 0) section = 2;
            else if (strncmp(trimmed, "Limitations:", 12) == 0) section = 3;
        } else if (section >= 1 && section <= 3) {
            if (strncmp(trimmed, "Conditions:", 11) == 0) { section = 2; continue; }
            if (strncmp(trimmed, "Limitations:", 12) == 0) { section = 3; continue; }
            
            if (trimmed[0] == '-') {
                char* item = trim(trimmed + 1);
                if (section == 1) add_to_list(&lic.permissions, &lic.num_permissions, item);
                else if (section == 2) add_to_list(&lic.conditions, &lic.num_conditions, item);
                else if (section == 3) add_to_list(&lic.limitations, &lic.num_limitations, item);
            }
        }
    }

    lic.template_text = strdup(full_text);
    free(full_text);
    fclose(f);

    if (lic.id) {
        if (license_count >= registry_capacity) {
            registry_capacity *= 2;
            registry = realloc(registry, registry_capacity * sizeof(License));
        }
        registry[license_count++] = lic;
    } else {
        free_license_data(&lic);
    }
}

void load_all_templates(const char* directory) {
    DIR* dir = opendir(directory);
    if (!dir) return;

    if (!registry) {
        registry = malloc(registry_capacity * sizeof(License));
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".template")) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
            parse_template_file(path);
        }
    }
    closedir(dir);
}

const License* get_license_by_id(const char* id) {
    for (int i = 0; i < license_count; i++) {
        if (strcmp(registry[i].id, id) == 0) return &registry[i];
    }
    return NULL;
}

const License* get_all_licenses(int* count) {
    *count = license_count;
    return registry;
}

void print_license_list(bool permissive, bool copyleft, bool public_domain) {
    bool group = !permissive && !copyleft && !public_domain;
    
    LicenseCategory cats[] = {CAT_PERMISSIVE, CAT_COPYLEFT, CAT_PUBLIC_DOMAIN, CAT_OTHER};
    const char* titles[] = {"Permissive", "Copyleft", "Public Domain", "Other"};
    
    printf("Available Licenses:\n");
    
    for (int i = 0; i < 4; i++) {
        bool show = group || 
                    (permissive && cats[i] == CAT_PERMISSIVE) ||
                    (copyleft && cats[i] == CAT_COPYLEFT) ||
                    (public_domain && cats[i] == CAT_PUBLIC_DOMAIN);
        
        if (!show) continue;
        
        bool found = false;
        for (int j = 0; j < license_count; j++) {
            if (registry[j].category == cats[i]) {
                if (!found && group) printf("\n%s%s%s\n", col(COL_BBLUE), titles[i], col(COL_RESET));
                printf("  %-12s %s\n", registry[j].id, registry[j].name);
                found = true;
            }
        }
    }
    printf("\nUse licgen <license> --help for details on a specific license.\n");
}
