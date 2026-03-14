/*
 * product.h - Product management declarations
 * Supply Chain & Warehouse Management System
 */

#ifndef PRODUCT_H
#define PRODUCT_H

#include "common.h"

/* ── Product record ──────────────────────────────────────────────────── */
typedef struct {
    int   id;
    char  name[MAX_NAME_LEN];
    char  category[MAX_CATEGORY_LEN];
    int   quantity;
    float price;
    int   supplier_id;
    char  warehouse_location[MAX_LOCATION_LEN];
} Product;

/* ── CRUD interface ──────────────────────────────────────────────────── */
void add_product(void);
void view_all_products(void);
void search_product(void);
void update_product(void);
void delete_product(void);

/* ── File I/O ────────────────────────────────────────────────────────── */
int load_products(Product products[], int *count);
int save_products(const Product products[], int count);

/* ── Lookup helper (used by other modules) ───────────────────────────── */
int find_product_by_id(const Product products[], int count, int id);

/* ── Sub-menu entry point ────────────────────────────────────────────── */
void product_menu(void);

#endif /* PRODUCT_H */
