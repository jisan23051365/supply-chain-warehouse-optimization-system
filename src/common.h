/*
 * common.h - Shared constants, macros, and utility function declarations
 * Supply Chain & Warehouse Management System
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

/* ── capacity limits ─────────────────────────────────────────────────── */
#define MAX_PRODUCTS      1000
#define MAX_SUPPLIERS      500
#define MAX_SHIPMENTS     2000

/* ── string field widths ─────────────────────────────────────────────── */
#define MAX_NAME_LEN       100
#define MAX_CATEGORY_LEN    50
#define MAX_LOCATION_LEN    50
#define MAX_EMAIL_LEN      100
#define MAX_PHONE_LEN       20
#define MAX_ADDR_LEN       200
#define MAX_DATE_LEN        20
#define MAX_STATUS_LEN      30
#define MAX_CONTACT_LEN    100

/* ── file paths ──────────────────────────────────────────────────────── */
#define DATA_DIR          "data"
#define PRODUCTS_FILE     "data/products.dat"
#define SUPPLIERS_FILE    "data/suppliers.dat"
#define SHIPMENTS_FILE    "data/shipments.dat"

/* ── boolean convenience ─────────────────────────────────────────────── */
#define TRUE  1
#define FALSE 0

/* ── utility function declarations ───────────────────────────────────── */
void clear_input_buffer(void);
void read_string(const char *prompt, char *buffer, int max_len);
int  read_int(const char *prompt);
float read_float(const char *prompt);
void ensure_data_dir(void);
void print_separator(void);
void print_header(const char *title);
void press_enter_to_continue(void);

#endif /* COMMON_H */
