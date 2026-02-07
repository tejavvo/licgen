#include "headers/help.h"
#include "headers/colors.h"
#include <stdio.h>

void print_usage(void) {
    printf("%sUsage:%s licgen <license> [options]\n", col(COL_BYELLOW), col(COL_RESET));
    printf("       licgen list [--permissive|--copyleft|--public]\n\n");
    
    printf("%sCommands:%s\n", col(COL_BYELLOW), col(COL_RESET));
    printf("  list              List all available licenses\n");
    printf("  <license>         Generate specified license\n\n");
    
    printf("%sOptions:%s\n", col(COL_BYELLOW), col(COL_RESET));
    printf("  --help, -h        Show help for a license or this message\n");
    printf("  --version, -v     Show version information\n");
    printf("  --name <name> Specify copyright holder name\n");
    printf("  --year <year> Specify copyright year\n");
    printf("  --output <file>, -o Write to file instead of stdout\n");
    printf("  --json, -j        Output in JSON format\n");
    printf("  --markdown, -m    Output in Markdown format (defaults to LICENSE.md)\n");
    printf("  --stdout, -s      Output to stdout instead of file\n");
    printf("  --no-color    Disable colored output\n");
    printf("  --brief       Output license text only\n\n");
    
    printf("%sExamples:%s\n", col(COL_BYELLOW), col(COL_RESET));
    printf("  licgen list                        # List all licenses\n");
    printf("  licgen mit --help                  # Show MIT license details\n");
    printf("  licgen mit                         # Generate MIT (interactive)\n");
    printf("  licgen mit --name \"John Doe\" --year 2026\n");
    printf("  licgen apache2 --output LICENSE\n");
    printf("  licgen list --json                 # JSON output for scripts\n");
}

void print_version(void) {
    printf("%slicgen%s version %s%s%s\n", 
           col(COL_BGREEN), col(COL_RESET),
           col(COL_BCYAN), LICGEN_VERSION, col(COL_RESET));
    printf("A modular license generator for your projects.\n");
    printf("https://github.com/teja/licgen\n");
}

void print_permission_item(const char* item) {
    printf("  %s✓%s %s\n", col(COL_BGREEN), col(COL_RESET), item);
}

void print_condition_item(const char* item) {
    printf("  %sⓘ%s %s\n", col(COL_BYELLOW), col(COL_RESET), item);
}

void print_limitation_item(const char* item) {
    printf("  %s✗%s %s\n", col(COL_BRED), col(COL_RESET), item);
}

void print_permissions(char** items, int count) {
    if (count == 0) return;
    printf("\n%sPermissions%s\n", col(COL_BGREEN), col(COL_RESET));
    for (int i = 0; i < count; i++) {
        print_permission_item(items[i]);
    }
}

void print_conditions(char** items, int count) {
    if (count == 0) return;
    printf("\n%sConditions%s\n", col(COL_BYELLOW), col(COL_RESET));
    for (int i = 0; i < count; i++) {
        print_condition_item(items[i]);
    }
}

void print_limitations(char** items, int count) {
    if (count == 0) return;
    printf("\n%sLimitations%s\n", col(COL_BRED), col(COL_RESET));
    for (int i = 0; i < count; i++) {
        print_limitation_item(items[i]);
    }
}

void print_license_help(const License* lic) {
    printf("\n%s%s%s (%s%s%s)\n", 
           col(COL_BWHITE), lic->name, col(COL_RESET),
           col(COL_BCYAN), lic->spdx_id, col(COL_RESET));
    printf("%s\n", lic->description);
    
    print_permissions(lic->permissions, lic->num_permissions);
    print_conditions(lic->conditions, lic->num_conditions);
    print_limitations(lic->limitations, lic->num_limitations);
    
    printf("\n%sDescription:%s\n", col(COL_BWHITE), col(COL_RESET));
    printf("  %s\n", lic->description);
}
