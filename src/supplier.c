/*
 * supplier.c - Supplier management implementation
 * Supply Chain & Warehouse Management System
 */

#include "supplier.h"

/* ── File I/O ────────────────────────────────────────────────────────── */

int load_suppliers(Supplier suppliers[], int *count)
{
    FILE *fp = fopen(SUPPLIERS_FILE, "rb");
    if (!fp) {
        *count = 0;
        return TRUE;
    }

    if (fread(count, sizeof(int), 1, fp) != 1) {
        *count = 0;
        fclose(fp);
        return TRUE;
    }

    if (*count < 0 || *count > MAX_SUPPLIERS) {
        *count = 0;
        fclose(fp);
        return FALSE;
    }

    if (*count > 0 &&
        fread(suppliers, sizeof(Supplier), (size_t)*count, fp) != (size_t)*count) {
        *count = 0;
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

int save_suppliers(const Supplier suppliers[], int count)
{
    ensure_data_dir();
    FILE *fp = fopen(SUPPLIERS_FILE, "wb");
    if (!fp) {
        perror("Cannot open suppliers file for writing");
        return FALSE;
    }

    if (fwrite(&count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return FALSE;
    }

    if (count > 0 &&
        fwrite(suppliers, sizeof(Supplier), (size_t)count, fp) != (size_t)count) {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

/* ── Lookup helper ───────────────────────────────────────────────────── */

int find_supplier_by_id(const Supplier suppliers[], int count, int id)
{
    for (int i = 0; i < count; i++) {
        if (suppliers[i].id == id)
            return i;
    }
    return -1;
}

/* ── Internal helpers ────────────────────────────────────────────────── */

static int next_supplier_id(const Supplier suppliers[], int count)
{
    int max_id = 0;
    for (int i = 0; i < count; i++) {
        if (suppliers[i].id > max_id)
            max_id = suppliers[i].id;
    }
    return max_id + 1;
}

static void print_supplier_row(const Supplier *s)
{
    printf("  ID      : %d\n",  s->id);
    printf("  Name    : %s\n",  s->name);
    printf("  Contact : %s\n",  s->contact);
    printf("  Address : %s\n",  s->address);
    printf("  Email   : %s\n",  s->email);
    printf("  Phone   : %s\n",  s->phone);
    printf("  ----------------------------------------\n");
}

/* ── CRUD ────────────────────────────────────────────────────────────── */

void add_supplier(void)
{
    Supplier suppliers[MAX_SUPPLIERS];
    int      count = 0;

    if (!load_suppliers(suppliers, &count)) {
        printf("Error loading suppliers data.\n");
        return;
    }

    if (count >= MAX_SUPPLIERS) {
        printf("Supplier storage is full (%d records).\n", MAX_SUPPLIERS);
        return;
    }

    Supplier s;
    s.id = next_supplier_id(suppliers, count);

    read_string("  Supplier Name    : ", s.name,    MAX_NAME_LEN);
    read_string("  Contact Person   : ", s.contact, MAX_CONTACT_LEN);
    read_string("  Address          : ", s.address, MAX_ADDR_LEN);
    read_string("  Email            : ", s.email,   MAX_EMAIL_LEN);
    read_string("  Phone            : ", s.phone,   MAX_PHONE_LEN);

    suppliers[count] = s;
    count++;

    if (save_suppliers(suppliers, count))
        printf("\n  Supplier added successfully (ID: %d).\n", s.id);
    else
        printf("\n  Error saving supplier.\n");
}

void view_all_suppliers(void)
{
    Supplier suppliers[MAX_SUPPLIERS];
    int      count = 0;

    if (!load_suppliers(suppliers, &count)) {
        printf("Error loading suppliers data.\n");
        return;
    }

    if (count == 0) {
        printf("  No suppliers found.\n");
        return;
    }

    printf("\n  Total suppliers: %d\n\n", count);
    for (int i = 0; i < count; i++)
        print_supplier_row(&suppliers[i]);
}

void search_supplier(void)
{
    Supplier suppliers[MAX_SUPPLIERS];
    int      count = 0;

    if (!load_suppliers(suppliers, &count)) {
        printf("Error loading suppliers data.\n");
        return;
    }

    printf("  1. Search by ID\n");
    printf("  2. Search by Name\n");
    int choice = read_int("  Choice: ");

    int found = 0;

    if (choice == 1) {
        int id  = read_int("  Enter Supplier ID: ");
        int idx = find_supplier_by_id(suppliers, count, id);
        if (idx >= 0) {
            printf("\n");
            print_supplier_row(&suppliers[idx]);
            found = 1;
        }
    } else if (choice == 2) {
        char name[MAX_NAME_LEN];
        read_string("  Enter Supplier Name (partial): ", name, MAX_NAME_LEN);
        for (int i = 0; i < count; i++) {
            char lower_name[MAX_NAME_LEN];
            char lower_query[MAX_NAME_LEN];
            size_t j;
            for (j = 0; j < strlen(suppliers[i].name); j++)
                lower_name[j] = (char)(suppliers[i].name[j] | 0x20);
            lower_name[j] = '\0';
            for (j = 0; j < strlen(name); j++)
                lower_query[j] = (char)(name[j] | 0x20);
            lower_query[j] = '\0';

            if (strstr(lower_name, lower_query)) {
                print_supplier_row(&suppliers[i]);
                found = 1;
            }
        }
    } else {
        printf("  Invalid choice.\n");
        return;
    }

    if (!found)
        printf("  No matching supplier found.\n");
}

void update_supplier(void)
{
    Supplier suppliers[MAX_SUPPLIERS];
    int      count = 0;

    if (!load_suppliers(suppliers, &count)) {
        printf("Error loading suppliers data.\n");
        return;
    }

    int id  = read_int("  Enter Supplier ID to update: ");
    int idx = find_supplier_by_id(suppliers, count, id);

    if (idx < 0) {
        printf("  Supplier with ID %d not found.\n", id);
        return;
    }

    printf("\n  Current values shown in [brackets]. Press Enter to keep.\n\n");

    Supplier *s = &suppliers[idx];
    char buf[MAX_ADDR_LEN];

    printf("  Name [%s]: ", s->name);
    fflush(stdout);
    if (fgets(buf, MAX_NAME_LEN, stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) strncpy(s->name, buf, MAX_NAME_LEN - 1);
    }

    printf("  Contact [%s]: ", s->contact);
    fflush(stdout);
    if (fgets(buf, MAX_CONTACT_LEN, stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) strncpy(s->contact, buf, MAX_CONTACT_LEN - 1);
    }

    printf("  Address [%s]: ", s->address);
    fflush(stdout);
    if (fgets(buf, MAX_ADDR_LEN, stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) strncpy(s->address, buf, MAX_ADDR_LEN - 1);
    }

    printf("  Email [%s]: ", s->email);
    fflush(stdout);
    if (fgets(buf, MAX_EMAIL_LEN, stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) strncpy(s->email, buf, MAX_EMAIL_LEN - 1);
    }

    printf("  Phone [%s]: ", s->phone);
    fflush(stdout);
    if (fgets(buf, MAX_PHONE_LEN, stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) strncpy(s->phone, buf, MAX_PHONE_LEN - 1);
    }

    if (save_suppliers(suppliers, count))
        printf("\n  Supplier updated successfully.\n");
    else
        printf("\n  Error saving supplier.\n");
}

void delete_supplier(void)
{
    Supplier suppliers[MAX_SUPPLIERS];
    int      count = 0;

    if (!load_suppliers(suppliers, &count)) {
        printf("Error loading suppliers data.\n");
        return;
    }

    int id  = read_int("  Enter Supplier ID to delete: ");
    int idx = find_supplier_by_id(suppliers, count, id);

    if (idx < 0) {
        printf("  Supplier with ID %d not found.\n", id);
        return;
    }

    printf("\n  About to delete: %s (ID: %d)\n", suppliers[idx].name, id);
    char confirm[4];
    read_string("  Confirm deletion? (yes/no): ", confirm, sizeof(confirm));

    if (strcmp(confirm, "yes") != 0) {
        printf("  Deletion cancelled.\n");
        return;
    }

    for (int i = idx; i < count - 1; i++)
        suppliers[i] = suppliers[i + 1];
    count--;

    if (save_suppliers(suppliers, count))
        printf("  Supplier deleted successfully.\n");
    else
        printf("  Error saving suppliers data.\n");
}

/* ── Sub-menu ─────────────────────────────────────────────────────────── */

void supplier_menu(void)
{
    int choice;
    do {
        printf("\n");
        print_header("SUPPLIER MANAGEMENT");
        printf("  1. Add Supplier\n");
        printf("  2. View All Suppliers\n");
        printf("  3. Search Supplier\n");
        printf("  4. Update Supplier\n");
        printf("  5. Delete Supplier\n");
        printf("  0. Back to Main Menu\n");
        print_separator();
        choice = read_int("  Enter choice: ");

        printf("\n");
        switch (choice) {
            case 1: add_supplier();      break;
            case 2: view_all_suppliers(); break;
            case 3: search_supplier();   break;
            case 4: update_supplier();   break;
            case 5: delete_supplier();   break;
            case 0: break;
            default: printf("  Invalid choice.\n");
        }

        if (choice != 0)
            press_enter_to_continue();

    } while (choice != 0);
}
