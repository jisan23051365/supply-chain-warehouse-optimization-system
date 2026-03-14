/*
 * analytics.c - Logistics cost analytics implementation
 * Supply Chain & Warehouse Management System
 */

#include "analytics.h"

/* ── Total logistics cost ────────────────────────────────────────────── */

void report_total_cost(void)
{
    Shipment shipments[MAX_SHIPMENTS];
    int      count = 0;

    if (!load_shipments(shipments, &count)) {
        printf("  Error loading shipments data.\n");
        return;
    }

    if (count == 0) {
        printf("  No shipments found.\n");
        return;
    }

    float total        = 0.0f;
    int   delivered    = 0;
    int   in_transit   = 0;
    int   pending      = 0;
    int   cancelled    = 0;

    for (int i = 0; i < count; i++) {
        total += shipments[i].shipping_cost;
        if      (strcmp(shipments[i].status, STATUS_DELIVERED)  == 0) delivered++;
        else if (strcmp(shipments[i].status, STATUS_IN_TRANSIT) == 0) in_transit++;
        else if (strcmp(shipments[i].status, STATUS_PENDING)    == 0) pending++;
        else if (strcmp(shipments[i].status, STATUS_CANCELLED)  == 0) cancelled++;
    }

    printf("\n  %-30s %d\n",     "Total Shipments:",       count);
    printf("  %-30s $%.2f\n",   "Total Logistics Cost:",  total);
    printf("  %-30s $%.2f\n",   "Average Cost/Shipment:", (count > 0 ? total / count : 0.0f));
    printf("\n  Status Breakdown:\n");
    printf("    %-20s %d\n", STATUS_DELIVERED,  delivered);
    printf("    %-20s %d\n", STATUS_IN_TRANSIT, in_transit);
    printf("    %-20s %d\n", STATUS_PENDING,    pending);
    printf("    %-20s %d\n", STATUS_CANCELLED,  cancelled);
}

/* ── Cost grouped by supplier ────────────────────────────────────────── */

void report_cost_by_supplier(void)
{
    Shipment shipments[MAX_SHIPMENTS];
    int      shp_count = 0;

    Supplier suppliers[MAX_SUPPLIERS];
    int      sup_count = 0;

    if (!load_shipments(shipments, &shp_count)) {
        printf("  Error loading shipments data.\n");
        return;
    }

    if (!load_suppliers(suppliers, &sup_count)) {
        printf("  Error loading suppliers data.\n");
        return;
    }

    if (shp_count == 0) {
        printf("  No shipments found.\n");
        return;
    }

    /* Collect unique supplier IDs referenced by shipments */
    int  seen_ids[MAX_SUPPLIERS];
    float costs[MAX_SUPPLIERS];
    int  shp_counts[MAX_SUPPLIERS];
    int  unique = 0;

    for (int i = 0; i < shp_count; i++) {
        int sid = shipments[i].supplier_id;
        int found = -1;
        for (int j = 0; j < unique; j++) {
            if (seen_ids[j] == sid) {
                found = j;
                break;
            }
        }
        if (found < 0) {
            if (unique >= MAX_SUPPLIERS) continue;
            seen_ids[unique]   = sid;
            costs[unique]      = shipments[i].shipping_cost;
            shp_counts[unique] = 1;
            unique++;
        } else {
            costs[found]      += shipments[i].shipping_cost;
            shp_counts[found] += 1;
        }
    }

    printf("\n  %-6s  %-25s  %-10s  %-12s\n",
           "Sup.ID", "Supplier Name", "Shipments", "Total Cost");
    printf("  %-6s  %-25s  %-10s  %-12s\n",
           "------", "-------------------------", "----------", "------------");

    for (int i = 0; i < unique; i++) {
        const char *name = "Unknown";
        int idx = find_supplier_by_id(suppliers, sup_count, seen_ids[i]);
        if (idx >= 0)
            name = suppliers[idx].name;

        printf("  %-6d  %-25.25s  %-10d  $%-11.2f\n",
               seen_ids[i], name, shp_counts[i], costs[i]);
    }
}

/* ── Cost grouped by product ─────────────────────────────────────────── */

void report_cost_by_product(void)
{
    Shipment shipments[MAX_SHIPMENTS];
    int      shp_count = 0;

    Product  products[MAX_PRODUCTS];
    int      prod_count = 0;

    if (!load_shipments(shipments, &shp_count)) {
        printf("  Error loading shipments data.\n");
        return;
    }

    if (!load_products(products, &prod_count)) {
        printf("  Error loading products data.\n");
        return;
    }

    if (shp_count == 0) {
        printf("  No shipments found.\n");
        return;
    }

    int   seen_ids[MAX_PRODUCTS];
    float costs[MAX_PRODUCTS];
    int   shp_counts[MAX_PRODUCTS];
    int   quantities[MAX_PRODUCTS];
    int   unique = 0;

    for (int i = 0; i < shp_count; i++) {
        int pid = shipments[i].product_id;
        int found = -1;
        for (int j = 0; j < unique; j++) {
            if (seen_ids[j] == pid) {
                found = j;
                break;
            }
        }
        if (found < 0) {
            if (unique >= MAX_PRODUCTS) continue;
            seen_ids[unique]   = pid;
            costs[unique]      = shipments[i].shipping_cost;
            shp_counts[unique] = 1;
            quantities[unique] = shipments[i].quantity;
            unique++;
        } else {
            costs[found]      += shipments[i].shipping_cost;
            shp_counts[found] += 1;
            quantities[found] += shipments[i].quantity;
        }
    }

    printf("\n  %-6s  %-25s  %-10s  %-10s  %-12s\n",
           "Prod.ID", "Product Name", "Shipments", "Total Qty", "Total Cost");
    printf("  %-6s  %-25s  %-10s  %-10s  %-12s\n",
           "-------", "-------------------------", "----------", "----------", "------------");

    for (int i = 0; i < unique; i++) {
        const char *name = "Unknown";
        int idx = find_product_by_id(products, prod_count, seen_ids[i]);
        if (idx >= 0)
            name = products[idx].name;

        printf("  %-6d  %-25.25s  %-10d  %-10d  $%-11.2f\n",
               seen_ids[i], name, shp_counts[i], quantities[i], costs[i]);
    }
}

/* ── Full summary report ─────────────────────────────────────────────── */

void report_shipment_summary(void)
{
    Shipment shipments[MAX_SHIPMENTS];
    int      shp_count = 0;

    Product  products[MAX_PRODUCTS];
    int      prod_count = 0;

    Supplier suppliers[MAX_SUPPLIERS];
    int      sup_count = 0;

    if (!load_shipments(shipments, &shp_count) ||
        !load_products(products,   &prod_count) ||
        !load_suppliers(suppliers,  &sup_count)) {
        printf("  Error loading data.\n");
        return;
    }

    printf("\n");
    print_header("SHIPMENT SUMMARY REPORT");
    printf("\n  Products registered : %d\n", prod_count);
    printf("  Suppliers registered: %d\n", sup_count);
    printf("  Shipments recorded  : %d\n\n", shp_count);

    float total_cost   = 0.0f;
    int   total_qty    = 0;
    float max_cost     = 0.0f;
    int   max_cost_id  = -1;

    for (int i = 0; i < shp_count; i++) {
        total_cost += shipments[i].shipping_cost;
        total_qty  += shipments[i].quantity;
        if (shipments[i].shipping_cost > max_cost) {
            max_cost    = shipments[i].shipping_cost;
            max_cost_id = shipments[i].id;
        }
    }

    printf("  Total units shipped    : %d\n",     total_qty);
    printf("  Total logistics cost   : $%.2f\n",  total_cost);
    printf("  Average cost/shipment  : $%.2f\n",
           (shp_count > 0 ? total_cost / shp_count : 0.0f));

    if (max_cost_id >= 0)
        printf("  Most expensive shipment: ID %d ($%.2f)\n", max_cost_id, max_cost);

    /* Status breakdown */
    int delivered = 0, in_transit = 0, pending = 0, cancelled = 0;
    for (int i = 0; i < shp_count; i++) {
        if      (strcmp(shipments[i].status, STATUS_DELIVERED)  == 0) delivered++;
        else if (strcmp(shipments[i].status, STATUS_IN_TRANSIT) == 0) in_transit++;
        else if (strcmp(shipments[i].status, STATUS_PENDING)    == 0) pending++;
        else                                                           cancelled++;
    }

    printf("\n  Shipment Status Breakdown:\n");
    printf("    Delivered  : %d (%.1f%%)\n", delivered,
           (shp_count > 0 ? 100.0f * delivered  / shp_count : 0.0f));
    printf("    In-Transit : %d (%.1f%%)\n", in_transit,
           (shp_count > 0 ? 100.0f * in_transit / shp_count : 0.0f));
    printf("    Pending    : %d (%.1f%%)\n", pending,
           (shp_count > 0 ? 100.0f * pending    / shp_count : 0.0f));
    printf("    Cancelled  : %d (%.1f%%)\n", cancelled,
           (shp_count > 0 ? 100.0f * cancelled  / shp_count : 0.0f));
}

/* ── Sub-menu ─────────────────────────────────────────────────────────── */

void analytics_menu(void)
{
    int choice;
    do {
        printf("\n");
        print_header("LOGISTICS COST ANALYTICS");
        printf("  1. Total Logistics Cost\n");
        printf("  2. Cost by Supplier\n");
        printf("  3. Cost by Product\n");
        printf("  4. Full Shipment Summary Report\n");
        printf("  0. Back to Main Menu\n");
        print_separator();
        choice = read_int("  Enter choice: ");

        printf("\n");
        switch (choice) {
            case 1: report_total_cost();       break;
            case 2: report_cost_by_supplier(); break;
            case 3: report_cost_by_product();  break;
            case 4: report_shipment_summary(); break;
            case 0: break;
            default: printf("  Invalid choice.\n");
        }

        if (choice != 0)
            press_enter_to_continue();

    } while (choice != 0);
}
