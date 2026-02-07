#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <dirent.h>

#include "headers/licgen.h"
#include "headers/colors.h"
#include "headers/prompt.h"
#include "headers/help.h"
#include "headers/licenses.h"
#include "headers/json.h"

/* Command line options */
static struct {
    const char* license_id;
    const char* name;
    const char* year;
    const char* output_file;
    bool show_help;
    bool show_version;
    bool json_output;
    bool brief;
    bool list_cmd;
    bool filter_permissive;
    bool filter_copyleft;
    bool filter_public;
    bool markdown_output;
    bool force_stdout;
} opts = {0};

static void parse_args(int argc, char* argv[]) {
    static struct option long_options[] = {
        {"help",       no_argument,       0, 'h'},
        {"version",    no_argument,       0, 'v'},
        {"name",       required_argument, 0, 'n'},
        {"year",       required_argument, 0, 'y'},
        {"output",     required_argument, 0, 'o'},
        {"json",       no_argument,       0, 'j'},
        {"no-color",   no_argument,       0, 'c'},
        {"brief",      no_argument,       0, 'b'},
        {"permissive", no_argument,       0, 'P'},
        {"copyleft",   no_argument,       0, 'C'},
        {"public",     no_argument,       0, 'D'},
        {"markdown",   no_argument,       0, 'm'},
        {"stdout",     no_argument,       0, 's'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "hvn:y:o:jcbms", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h': opts.show_help = true; break;
            case 'v': opts.show_version = true; break;
            case 'n': opts.name = optarg; break;
            case 'y': opts.year = optarg; break;
            case 'o': opts.output_file = optarg; break;
            case 'j': opts.json_output = true; break;
            case 'P': opts.filter_permissive = true; break;
            case 'C': opts.filter_copyleft = true; break;
            case 'D': opts.filter_public = true; break;
            case 'm': opts.markdown_output = true; break;
            case 's': opts.force_stdout = true; break;
        }
    }
    
    /* Get positional argument (license id or "list") */
    if (optind < argc) {
        if (strcmp(argv[optind], "list") == 0) {
            opts.list_cmd = true;
        } else {
            opts.license_id = argv[optind];
        }
    }
}

static int handle_list(void) {
    if (opts.json_output) {
        int count;
        const License* licenses = get_all_licenses(&count);
        print_json_list(licenses, count, stdout);
    } else {
        print_license_list(opts.filter_permissive, opts.filter_copyleft, opts.filter_public);
    }
    return 0;
}

static char* replace_placeholders(const char* text, const char* name, const char* year) {
    if (!text) return NULL;
    char* result = malloc(MAX_LICENSE_TEXT);
    char* out = result;
    const char* in = text;
    
    while (*in && (out - result < MAX_LICENSE_TEXT - 1)) {
        if (strncmp(in, "{{name}}", 8) == 0) {
            if (name) {
                strcpy(out, name);
                out += strlen(name);
            }
            in += 8;
        } else if (strncmp(in, "{{year}}", 8) == 0) {
            if (year) {
                strcpy(out, year);
                out += strlen(year);
            }
            in += 8;
        } else {
            *out++ = *in++;
        }
    }
    *out = '\0';
    return result;
}

static int handle_license_help(const License* lic) {
    print_license_help(lic);
    return 0;
}


static int handle_license_generate(const License* lic) {
    char* name = NULL;
    char* year = NULL;
    
    /* Get required fields interactively or from args */
    if (lic->needs_name) {
        if (opts.name) {
            name = safe_strdup(opts.name);
        } else {
            name = prompt_string("Copyright holder name", NULL);
            if (!name || !*name) {
                fprintf(stderr, "Error: Name is required for %s license.\n", lic->name);
                return 1;
            }
        }
    }
    
    if (lic->needs_year) {
        if (opts.year) {
            year = safe_strdup(opts.year);
        } else {
            year = prompt_year();
        }
    }
    
    /* Generate license text */
    char* text = replace_placeholders(lic->template_text, name, year);
    if (!text) {
        fprintf(stderr, "Error: Failed to generate license text.\n");
        free(name);
        free(year);
        return 1;
    }
    
    /* Output */
    FILE* out = stdout;
    const char* final_output = opts.output_file;
    
    if (opts.force_stdout) {
        final_output = NULL;
    } else if (!final_output) {
        if (opts.json_output) final_output = "LICENSE.json";
        else if (opts.markdown_output) final_output = "LICENSE.md";
        else final_output = "LICENSE.txt";
    }

    if (final_output) {
        out = fopen(final_output, "w");
        if (!out) {
            fprintf(stderr, "Error: Cannot open file '%s' for writing.\n", final_output);
            free(name);
            free(year);
            free(text);
            return 1;
        }
    }

    if (opts.json_output) {
        print_json_license(lic, text, out);
    } else {
        
        if (!opts.brief && out == stdout) {
            printf("\n%s%s%s\n", col(COL_DIM), "─────────────────────────────────────", col(COL_RESET));
        }
        
        fprintf(out, "%s\n", text);
        
        if (!opts.brief && out == stdout) {
            printf("%s%s%s\n", col(COL_DIM), "─────────────────────────────────────", col(COL_RESET));
        }
        
        if (final_output) {
            fclose(out);
            printf("License written to %s%s%s\n", col(COL_BGREEN), final_output, col(COL_RESET));
        }
    }
    
    free(name);
    free(year);
    free(text);
    return 0;
}

int main(int argc, char* argv[]) {
    colors_init();
    
    /* Try local templates first, then system path */
    DIR* dir = opendir("templates");
    if (dir) {
        closedir(dir);
        load_all_templates("templates");
    } else {
        load_all_templates(TEMPLATE_DIR);
    }
    
    parse_args(argc, argv);
    
    /* Handle version */
    if (opts.show_version) {
        print_version();
        free_licenses();
        return 0;
    }
    
    /* Handle list command */
    if (opts.list_cmd) {
        int ret = handle_list();
        free_licenses();
        return ret;
    }
    
    /* Handle general help */
    if (opts.show_help && !opts.license_id) {
        print_usage();
        free_licenses();
        return 0;
    }
    
    /* Need a license ID from here */
    if (!opts.license_id) {
        print_usage();
        free_licenses();
        return 1;
    }
    
    /* Look up the license */
    const License* lic = get_license_by_id(opts.license_id);
    if (!lic) {
        fprintf(stderr, "Unknown license: %s\n", opts.license_id);
        fprintf(stderr, "Use 'licgen list' to see available licenses.\n");
        free_licenses();
        return 1;
    }
    
    /* Handle license-specific help */
    if (opts.show_help) {
        int ret = handle_license_help(lic);
        free_licenses();
        return ret;
    }
    
    /* Generate license */
    int ret = handle_license_generate(lic);
    free_licenses();
    return ret;
}
