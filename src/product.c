/*
 * product.c - Product management implementation
 * Supply Chain & Warehouse Management System
 */

#include <ctype.h>
#include "product.h"

/* ── File I/O ────────────────────────────────────────────────────────── */

int load_products(Product products[], int *count)
{
    FILE *fp = fopen(PRODUCTS_FILE, "rb");
    if (!fp) {
        *count = 0;
        return TRUE; /* first run — no file yet */
    }

    if (fread(count, sizeof(int), 1, fp) != 1) {
        *count = 0;
        fclose(fp);
        return TRUE;
    }

    if (*count < 0 || *count > MAX_PRODUCTS) {
        *count = 0;
        fclose(fp);
        return FALSE;
    }

    if (*count > 0 &&
        fread(products, sizeof(Product), (size_t)*count, fp) != (size_t)*count) {
        *count = 0;
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

int save_products(const Product products[], int count)
{
    ensure_data_dir();
    FILE *fp = fopen(PRODUCTS_FILE, "wb");
    if (!fp) {
        perror("Cannot open products file for writing");
        return FALSE;
    }

    if (fwrite(&count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return FALSE;
    }

    if (count > 0 &&
        fwrite(products, sizeof(Product), (size_t)count, fp) != (size_t)count) {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

/* ── Lookup helper ───────────────────────────────────────────────────── */

int find_product_by_id(const Product products[], int count, int id)
{
    for (int i = 0; i < count; i++) {
        if (products[i].id == id)
            return i;
    }
    return -1;
}

/* ── Internal: next available ID ─────────────────────────────────────── */

static int next_product_id(const Product products[], int count)
{
    int max_id = 0;
    for (int i = 0; i < count; i++) {
        if (products[i].id > max_id)
            max_id = products[i].id;
    }
    return max_id + 1;
}

/* ── Internal: print a single product row ────────────────────────────── */

static void print_product_row(const Product *p)
{
    printf("  ID          : %d\n",     p->id);
    printf("  Name        : %s\n",     p->name);
    printf("  Category    : %s\n",     p->category);
    printf("  Quantity    : %d\n",     p->quantity);
    printf("  Price       : $%.2f\n",  p->price);
    printf("  Supplier ID : %d\n",     p->supplier_id);
    printf("  Location    : %s\n",     p->warehouse_location);
    printf("  ----------------------------------------\n");
}

/* ── CRUD ────────────────────────────────────────────────────────────── */

void add_product(void)
{
    Product products[MAX_PRODUCTS];
    int     count = 0;

    if (!load_products(products, &count)) {
        printf("Error loading products data.\n");
        return;
    }

    if (count >= MAX_PRODUCTS) {
        printf("Product storage is full (%d records).\n", MAX_PRODUCTS);
        return;
    }

    Product p;
    p.id = next_product_id(products, count);

    read_string("  Product Name        : ", p.name,               MAX_NAME_LEN);
    read_string("  Category            : ", p.category,           MAX_CATEGORY_LEN);
    p.quantity    = read_int  ("  Quantity            : ");
    p.price       = read_float("  Unit Price ($)      : ");
    p.supplier_id = read_int  ("  Supplier ID         : ");
    read_string("  Warehouse Location  : ", p.warehouse_location, MAX_LOCATION_LEN);

    if (p.quantity < 0) {
        printf("Quantity cannot be negative.\n");
        return;
    }

    products[count] = p;
    count++;

    if (save_products(products, count))
        printf("\n  Product added successfully (ID: %d).\n", p.id);
    else
        printf("\n  Error saving product.\n");
}

void view_all_products(void)
{
    Product products[MAX_PRODUCTS];
    int     count = 0;

    if (!load_products(products, &count)) {
        printf("Error loading products data.\n");
        return;
    }

    if (count == 0) {
        printf("  No products found.\n");
        return;
    }

    printf("\n  Total products: %d\n\n", count);
    for (int i = 0; i < count; i++)
        print_product_row(&products[i]);
}

void search_product(void)
{
    Product products[MAX_PRODUCTS];
    int     count = 0;

    if (!load_products(products, &count)) {
        printf("Error loading products data.\n");
        return;
    }

    printf("  1. Search by ID\n");
    printf("  2. Search by Name\n");
    int choice = read_int("  Choice: ");

    int found = 0;

    if (choice == 1) {
        int id = read_int("  Enter Product ID: ");
        int idx = find_product_by_id(products, count, id);
        if (idx >= 0) {
            printf("\n");
            print_product_row(&products[idx]);
            found = 1;
        }
    } else if (choice == 2) {
        char name[MAX_NAME_LEN];
        read_string("  Enter Product Name (partial): ", name, MAX_NAME_LEN);
        /* Case-insensitive substring search */
        for (int i = 0; i < count; i++) {
            char lower_name[MAX_NAME_LEN];
            char lower_query[MAX_NAME_LEN];
            size_t name_len  = strlen(products[i].name);
            size_t query_len = strlen(name);
            size_t j;
            for (j = 0; j < name_len; j++)
                lower_name[j] = (char)tolower((unsigned char)products[i].name[j]);
            lower_name[j] = '\0';
            for (j = 0; j < query_len; j++)
                lower_query[j] = (char)tolower((unsigned char)name[j]);
            lower_query[j] = '\0';

            if (strstr(lower_name, lower_query)) {
                print_product_row(&products[i]);
                found = 1;
            }
        }
    } else {
        printf("  Invalid choice.\n");
        return;
    }

    if (!found)
        printf("  No matching product found.\n");
}

void update_product(void)
{
    Product products[MAX_PRODUCTS];
    int     count = 0;

    if (!load_products(products, &count)) {
        printf("Error loading products data.\n");
        return;
    }

    int id  = read_int("  Enter Product ID to update: ");
    int idx = find_product_by_id(products, count, id);

    if (idx < 0) {
        printf("  Product with ID %d not found.\n", id);
        return;
    }

    printf("\n  Current values shown in [brackets]. Press Enter to keep.\n\n");

    Product *p = &products[idx];
    char buf[MAX_NAME_LEN];

    printf("  Name [%s]: ", p->name);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) strncpy(p->name, buf, MAX_NAME_LEN - 1);
    }

    printf("  Category [%s]: ", p->category);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) strncpy(p->category, buf, MAX_CATEGORY_LEN - 1);
    }

    printf("  Quantity [%d]: ", p->quantity);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) {
            char *endptr;
            int q = (int)strtol(buf, &endptr, 10);
            if (endptr != buf && *endptr == '\0' && q >= 0)
                p->quantity = q;
        }
    }

    printf("  Price [%.2f]: ", p->price);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) {
            char *endptr;
            float price = strtof(buf, &endptr);
            if (endptr != buf && *endptr == '\0' && price >= 0.0f)
                p->price = price;
        }
    }

    printf("  Supplier ID [%d]: ", p->supplier_id);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) {
            char *endptr;
            int sid = (int)strtol(buf, &endptr, 10);
            if (endptr != buf && *endptr == '\0')
                p->supplier_id = sid;
        }
    }

    printf("  Warehouse Location [%s]: ", p->warehouse_location);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) strncpy(p->warehouse_location, buf, MAX_LOCATION_LEN - 1);
    }

    if (save_products(products, count))
        printf("\n  Product updated successfully.\n");
    else
        printf("\n  Error saving product.\n");
}

void delete_product(void)
{
    Product products[MAX_PRODUCTS];
    int     count = 0;

    if (!load_products(products, &count)) {
        printf("Error loading products data.\n");
        return;
    }

    int id  = read_int("  Enter Product ID to delete: ");
    int idx = find_product_by_id(products, count, id);

    if (idx < 0) {
        printf("  Product with ID %d not found.\n", id);
        return;
    }

    printf("\n  About to delete: %s (ID: %d)\n", products[idx].name, id);
    char confirm[8];
    read_string("  Confirm deletion? (yes/no): ", confirm, sizeof(confirm));

    if (strcmp(confirm, "yes") != 0) {
        printf("  Deletion cancelled.\n");
        return;
    }

    /* Shift remaining records left */
    for (int i = idx; i < count - 1; i++)
        products[i] = products[i + 1];
    count--;

    if (save_products(products, count))
        printf("  Product deleted successfully.\n");
    else
        printf("  Error saving products data.\n");
}

/* ── Sub-menu ─────────────────────────────────────────────────────────── */

void product_menu(void)
{
    int choice;
    do {
        printf("\n");
        print_header("PRODUCT MANAGEMENT");
        printf("  1. Add Product\n");
        printf("  2. View All Products\n");
        printf("  3. Search Product\n");
        printf("  4. Update Product\n");
        printf("  5. Delete Product\n");
        printf("  0. Back to Main Menu\n");
        print_separator();
        choice = read_int("  Enter choice: ");

        printf("\n");
        switch (choice) {
            case 1: add_product();    break;
            case 2: view_all_products(); break;
            case 3: search_product(); break;
            case 4: update_product(); break;
            case 5: delete_product(); break;
            case 0: break;
            default: printf("  Invalid choice.\n");
        }

        if (choice != 0)
            press_enter_to_continue();

    } while (choice != 0);
}
