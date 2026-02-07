#ifndef PROMPT_H
#define PROMPT_H

/* Prompt user for a string input */
char* prompt_string(const char* prompt, const char* default_value);

/* Prompt user for year (defaults to current year) */
char* prompt_year(void);

/* Get current year as string */
char* get_current_year(void);

#endif /* PROMPT_H */
