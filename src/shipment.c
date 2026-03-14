/*
 * shipment.c - Shipment tracking implementation
 * Supply Chain & Warehouse Management System
 */

#include "shipment.h"

/* ── File I/O ────────────────────────────────────────────────────────── */

int load_shipments(Shipment shipments[], int *count)
{
    FILE *fp = fopen(SHIPMENTS_FILE, "rb");
    if (!fp) {
        *count = 0;
        return TRUE;
    }

    if (fread(count, sizeof(int), 1, fp) != 1) {
        *count = 0;
        fclose(fp);
        return TRUE;
    }

    if (*count < 0 || *count > MAX_SHIPMENTS) {
        *count = 0;
        fclose(fp);
        return FALSE;
    }

    if (*count > 0 &&
        fread(shipments, sizeof(Shipment), (size_t)*count, fp) != (size_t)*count) {
        *count = 0;
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

int save_shipments(const Shipment shipments[], int count)
{
    ensure_data_dir();
    FILE *fp = fopen(SHIPMENTS_FILE, "wb");
    if (!fp) {
        perror("Cannot open shipments file for writing");
        return FALSE;
    }

    if (fwrite(&count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return FALSE;
    }

    if (count > 0 &&
        fwrite(shipments, sizeof(Shipment), (size_t)count, fp) != (size_t)count) {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

/* ── Internal helpers ────────────────────────────────────────────────── */

static int next_shipment_id(const Shipment shipments[], int count)
{
    int max_id = 0;
    for (int i = 0; i < count; i++) {
        if (shipments[i].id > max_id)
            max_id = shipments[i].id;
    }
    return max_id + 1;
}

static int find_shipment_by_id(const Shipment shipments[], int count, int id)
{
    for (int i = 0; i < count; i++) {
        if (shipments[i].id == id)
            return i;
    }
    return -1;
}

static void print_shipment_row(const Shipment *s)
{
    printf("  Shipment ID      : %d\n",    s->id);
    printf("  Product ID       : %d\n",    s->product_id);
    printf("  Supplier ID      : %d\n",    s->supplier_id);
    printf("  Quantity         : %d\n",    s->quantity);
    printf("  Shipment Date    : %s\n",    s->shipment_date);
    printf("  Expected Arrival : %s\n",    s->expected_arrival);
    printf("  Status           : %s\n",    s->status);
    printf("  Shipping Cost    : $%.2f\n", s->shipping_cost);
    printf("  ----------------------------------------\n");
}

static int valid_status(const char *status)
{
    return (strcmp(status, STATUS_PENDING)    == 0 ||
            strcmp(status, STATUS_IN_TRANSIT) == 0 ||
            strcmp(status, STATUS_DELIVERED)  == 0 ||
            strcmp(status, STATUS_CANCELLED)  == 0);
}

/* ── CRUD ────────────────────────────────────────────────────────────── */

void create_shipment(void)
{
    Shipment shipments[MAX_SHIPMENTS];
    int      count = 0;

    if (!load_shipments(shipments, &count)) {
        printf("Error loading shipments data.\n");
        return;
    }

    if (count >= MAX_SHIPMENTS) {
        printf("Shipment storage is full (%d records).\n", MAX_SHIPMENTS);
        return;
    }

    Shipment s;
    s.id = next_shipment_id(shipments, count);

    s.product_id  = read_int  ("  Product ID          : ");
    s.supplier_id = read_int  ("  Supplier ID         : ");
    s.quantity    = read_int  ("  Quantity            : ");
    if (s.quantity <= 0) {
        printf("  Quantity must be positive.\n");
        return;
    }
    read_string("  Shipment Date (YYYY-MM-DD)  : ", s.shipment_date,   MAX_DATE_LEN);
    read_string("  Expected Arrival (YYYY-MM-DD): ", s.expected_arrival, MAX_DATE_LEN);

    printf("  Status options: %s | %s | %s | %s\n",
           STATUS_PENDING, STATUS_IN_TRANSIT, STATUS_DELIVERED, STATUS_CANCELLED);
    do {
        read_string("  Status : ", s.status, MAX_STATUS_LEN);
        if (!valid_status(s.status))
            printf("  Invalid status. Please choose from the options above.\n");
    } while (!valid_status(s.status));

    s.shipping_cost = read_float("  Shipping Cost ($)   : ");

    shipments[count] = s;
    count++;

    if (save_shipments(shipments, count))
        printf("\n  Shipment created successfully (ID: %d).\n", s.id);
    else
        printf("\n  Error saving shipment.\n");
}

void view_all_shipments(void)
{
    Shipment shipments[MAX_SHIPMENTS];
    int      count = 0;

    if (!load_shipments(shipments, &count)) {
        printf("Error loading shipments data.\n");
        return;
    }

    if (count == 0) {
        printf("  No shipments found.\n");
        return;
    }

    printf("\n  Total shipments: %d\n\n", count);
    for (int i = 0; i < count; i++)
        print_shipment_row(&shipments[i]);
}

void track_shipment(void)
{
    Shipment shipments[MAX_SHIPMENTS];
    int      count = 0;

    if (!load_shipments(shipments, &count)) {
        printf("Error loading shipments data.\n");
        return;
    }

    int id  = read_int("  Enter Shipment ID: ");
    int idx = find_shipment_by_id(shipments, count, id);

    if (idx < 0) {
        printf("  Shipment with ID %d not found.\n", id);
        return;
    }

    printf("\n");
    print_shipment_row(&shipments[idx]);
}

void update_shipment_status(void)
{
    Shipment shipments[MAX_SHIPMENTS];
    int      count = 0;

    if (!load_shipments(shipments, &count)) {
        printf("Error loading shipments data.\n");
        return;
    }

    int id  = read_int("  Enter Shipment ID to update: ");
    int idx = find_shipment_by_id(shipments, count, id);

    if (idx < 0) {
        printf("  Shipment with ID %d not found.\n", id);
        return;
    }

    Shipment *s = &shipments[idx];
    printf("\n  Current status: %s\n", s->status);
    printf("  Status options: %s | %s | %s | %s\n",
           STATUS_PENDING, STATUS_IN_TRANSIT, STATUS_DELIVERED, STATUS_CANCELLED);

    char new_status[MAX_STATUS_LEN];
    do {
        read_string("  New Status: ", new_status, MAX_STATUS_LEN);
        if (!valid_status(new_status))
            printf("  Invalid status. Please choose from the options above.\n");
    } while (!valid_status(new_status));

    strncpy(s->status, new_status, MAX_STATUS_LEN - 1);
    s->status[MAX_STATUS_LEN - 1] = '\0';

    /* Also allow updating shipping cost */
    printf("  Current shipping cost: $%.2f\n", s->shipping_cost);
    printf("  New shipping cost (press Enter to keep): ");
    fflush(stdout);
    char buf[32];
    if (fgets(buf, sizeof(buf), stdin)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (strlen(buf) > 0) {
            char *endptr;
            float cost = strtof(buf, &endptr);
            if (endptr != buf && *endptr == '\0' && cost >= 0.0f)
                s->shipping_cost = cost;
        }
    }

    if (save_shipments(shipments, count))
        printf("\n  Shipment updated successfully.\n");
    else
        printf("\n  Error saving shipment.\n");
}

void delete_shipment(void)
{
    Shipment shipments[MAX_SHIPMENTS];
    int      count = 0;

    if (!load_shipments(shipments, &count)) {
        printf("Error loading shipments data.\n");
        return;
    }

    int id  = read_int("  Enter Shipment ID to delete: ");
    int idx = find_shipment_by_id(shipments, count, id);

    if (idx < 0) {
        printf("  Shipment with ID %d not found.\n", id);
        return;
    }

    printf("\n  About to delete Shipment ID %d (Product: %d, Supplier: %d)\n",
           id, shipments[idx].product_id, shipments[idx].supplier_id);
    char confirm[8];
    read_string("  Confirm deletion? (yes/no): ", confirm, sizeof(confirm));

    if (strcmp(confirm, "yes") != 0) {
        printf("  Deletion cancelled.\n");
        return;
    }

    for (int i = idx; i < count - 1; i++)
        shipments[i] = shipments[i + 1];
    count--;

    if (save_shipments(shipments, count))
        printf("  Shipment deleted successfully.\n");
    else
        printf("  Error saving shipments data.\n");
}

/* ── Sub-menu ─────────────────────────────────────────────────────────── */

void shipment_menu(void)
{
    int choice;
    do {
        printf("\n");
        print_header("SHIPMENT TRACKING");
        printf("  1. Create Shipment\n");
        printf("  2. View All Shipments\n");
        printf("  3. Track Shipment by ID\n");
        printf("  4. Update Shipment Status / Cost\n");
        printf("  5. Delete Shipment\n");
        printf("  0. Back to Main Menu\n");
        print_separator();
        choice = read_int("  Enter choice: ");

        printf("\n");
        switch (choice) {
            case 1: create_shipment();        break;
            case 2: view_all_shipments();     break;
            case 3: track_shipment();         break;
            case 4: update_shipment_status(); break;
            case 5: delete_shipment();        break;
            case 0: break;
            default: printf("  Invalid choice.\n");
        }

        if (choice != 0)
            press_enter_to_continue();

    } while (choice != 0);
}
