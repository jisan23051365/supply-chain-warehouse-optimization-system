/*
 * common.c - Utility function implementations
 * Supply Chain & Warehouse Management System
 */

#include "common.h"

/* Clear any leftover characters in stdin */
void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/*
 * Safely read a non-empty string from stdin.
 * Strips the trailing newline and enforces max_len - 1 characters.
 */
void read_string(const char *prompt, char *buffer, int max_len)
{
    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(buffer, max_len, stdin) == NULL) {
            buffer[0] = '\0';
            return;
        }

        /* Strip trailing newline */
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        } else {
            /* Input was longer than max_len — discard the rest */
            clear_input_buffer();
        }

        if (len > 0)
            return;

        printf("  Input cannot be empty. Please try again.\n");
    }
}

/* Read a validated integer from stdin */
int read_int(const char *prompt)
{
    char buf[32];
    int  value;
    char *endptr;

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            return 0;
        }

        /* Trim newline */
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        else
            clear_input_buffer();

        errno  = 0;
        value  = (int)strtol(buf, &endptr, 10);
        if (endptr != buf && *endptr == '\0' && errno == 0)
            return value;

        printf("  Invalid input. Please enter an integer.\n");
    }
}

/* Read a validated non-negative float from stdin */
float read_float(const char *prompt)
{
    char  buf[32];
    float value;
    char *endptr;

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            return 0.0f;
        }

        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        else
            clear_input_buffer();

        errno = 0;
        value = strtof(buf, &endptr);
        if (endptr != buf && *endptr == '\0' && errno == 0 && value >= 0.0f)
            return value;

        printf("  Invalid input. Please enter a non-negative number.\n");
    }
}

/* Create the data directory if it does not yet exist */
void ensure_data_dir(void)
{
    struct stat st = {0};
    if (stat(DATA_DIR, &st) == -1) {
        if (mkdir(DATA_DIR, 0755) != 0) {
            perror("Failed to create data directory");
        }
    }
}

/* Print a full-width horizontal rule */
void print_separator(void)
{
    printf("================================================================\n");
}

/* Print a centred section header */
void print_header(const char *title)
{
    print_separator();
    printf("  %s\n", title);
    print_separator();
}

/* Pause until the user presses Enter */
void press_enter_to_continue(void)
{
    printf("\nPress Enter to continue...");
    fflush(stdout);
    clear_input_buffer();
}
