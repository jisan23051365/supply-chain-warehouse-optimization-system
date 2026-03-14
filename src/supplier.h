/*
 * supplier.h - Supplier management declarations
 * Supply Chain & Warehouse Management System
 */

#ifndef SUPPLIER_H
#define SUPPLIER_H

#include "common.h"

/* ── Supplier record ─────────────────────────────────────────────────── */
typedef struct {
    int  id;
    char name[MAX_NAME_LEN];
    char contact[MAX_CONTACT_LEN];
    char address[MAX_ADDR_LEN];
    char email[MAX_EMAIL_LEN];
    char phone[MAX_PHONE_LEN];
} Supplier;

/* ── CRUD interface ──────────────────────────────────────────────────── */
void add_supplier(void);
void view_all_suppliers(void);
void search_supplier(void);
void update_supplier(void);
void delete_supplier(void);

/* ── File I/O ────────────────────────────────────────────────────────── */
int load_suppliers(Supplier suppliers[], int *count);
int save_suppliers(const Supplier suppliers[], int count);

/* ── Lookup helper (used by other modules) ───────────────────────────── */
int find_supplier_by_id(const Supplier suppliers[], int count, int id);

/* ── Sub-menu entry point ────────────────────────────────────────────── */
void supplier_menu(void);

#endif /* SUPPLIER_H */
