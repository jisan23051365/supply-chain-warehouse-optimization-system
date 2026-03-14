/*
 * main.c - Application entry point and main menu
 * Supply Chain & Warehouse Management System
 */

#include "common.h"
#include "product.h"
#include "supplier.h"
#include "shipment.h"
#include "analytics.h"

static void print_main_menu(void)
{
    printf("\n");
    print_header("SUPPLY CHAIN & WAREHOUSE MANAGEMENT SYSTEM");
    printf("  1. Product Management\n");
    printf("  2. Supplier Management\n");
    printf("  3. Shipment Tracking\n");
    printf("  4. Logistics Cost Analytics\n");
    printf("  0. Exit\n");
    print_separator();
}

int main(void)
{
    ensure_data_dir();

    int choice;
    do {
        print_main_menu();
        choice = read_int("  Enter choice: ");

        switch (choice) {
            case 1: product_menu();   break;
            case 2: supplier_menu();  break;
            case 3: shipment_menu();  break;
            case 4: analytics_menu(); break;
            case 0:
                printf("\n  Goodbye!\n\n");
                break;
            default:
                printf("\n  Invalid choice. Please select 0-4.\n");
        }
    } while (choice != 0);

    return 0;
}
