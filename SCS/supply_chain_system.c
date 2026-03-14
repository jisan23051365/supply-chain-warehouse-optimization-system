#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* =========================================================
   SUPPLY CHAIN & WAREHOUSE OPTIMIZATION SYSTEM
   ========================================================= */

/* ===================== STRUCTURES ===================== */

typedef struct {
    int id;
    char username[30];
    unsigned long password_hash;
} User;

typedef struct {
    int id;
    char name[50];
    int quantity;
    float cost_per_unit;
    int reorder_level;
} Product;

typedef struct {
    int id;
    char supplier_name[50];
    float reliability_score;
} Supplier;

typedef struct {
    int product_id;
    int quantity;
    float total_cost;
    char timestamp[30];
} Shipment;

/* ===================== UTILITIES ===================== */

unsigned long hashPassword(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

void logActivity(const char *msg) {
    FILE *fp = fopen("supply_logs.txt", "a");
    if (!fp) return;
    time_t now = time(NULL);
    fprintf(fp, "%s - %s\n", ctime(&now), msg);
    fprintf(fp, "--------------------------------\n");
    fclose(fp);
}

/* ===================== AUTH ===================== */

void initializeAdmin() {
    FILE *fp = fopen("supply_users.dat", "rb");
    if (fp) { fclose(fp); return; }

    fp = fopen("supply_users.dat", "wb");
    User admin = {1, "admin", hashPassword("admin123")};
    fwrite(&admin, sizeof(User), 1, fp);
    fclose(fp);
}

int login() {
    char username[30], password[30];
    unsigned long hash;
    User u;

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    hash = hashPassword(password);

    FILE *fp = fopen("supply_users.dat", "rb");
    if (!fp) return 0;

    while (fread(&u, sizeof(User), 1, fp)) {
        if (strcmp(u.username, username) == 0 && u.password_hash == hash) {
            fclose(fp);
            logActivity("Admin login successful.");
            return 1;
        }
    }

    fclose(fp);
    printf("Invalid credentials.\n");
    return 0;
}

/* ===================== PRODUCT ===================== */

void addProduct() {
    FILE *fp = fopen("products.dat", "ab");
    if (!fp) return;

    Product p;
    printf("Product ID: "); scanf("%d", &p.id);
    printf("Product Name: "); scanf(" %[^\n]", p.name);
    printf("Quantity: "); scanf("%d", &p.quantity);
    printf("Cost Per Unit: "); scanf("%f", &p.cost_per_unit);
    printf("Reorder Level: "); scanf("%d", &p.reorder_level);

    fwrite(&p, sizeof(Product), 1, fp);
    fclose(fp);
    logActivity("Product added.");
}

void viewProducts() {
    FILE *fp = fopen("products.dat", "rb");
    if (!fp) return;

    Product p;
    printf("\n--- Product Inventory ---\n");
    while (fread(&p, sizeof(Product), 1, fp)) {
        printf("ID:%d | %s | Qty:%d | Cost:%.2f",
               p.id, p.name, p.quantity, p.cost_per_unit);
        if (p.quantity <= p.reorder_level)
            printf("  **REORDER REQUIRED**");
        printf("\n");
    }
    fclose(fp);
}

/* ===================== SUPPLIER ===================== */

void addSupplier() {
    FILE *fp = fopen("suppliers.dat", "ab");
    if (!fp) return;

    Supplier s;
    printf("Supplier ID: "); scanf("%d", &s.id);
    printf("Supplier Name: "); scanf(" %[^\n]", s.supplier_name);
    printf("Reliability Score (0-100): "); scanf("%f", &s.reliability_score);

    fwrite(&s, sizeof(Supplier), 1, fp);
    fclose(fp);
    logActivity("Supplier added.");
}

/* ===================== SHIPMENT ===================== */

void recordShipment() {
    FILE *pfp = fopen("products.dat", "rb+");
    FILE *sfp = fopen("shipments.dat", "ab");
    if (!pfp || !sfp) return;

    int id, qty;
    Product p;

    printf("Product ID: "); scanf("%d", &id);
    printf("Shipment Quantity: "); scanf("%d", &qty);

    while (fread(&p, sizeof(Product), 1, pfp)) {
        if (p.id == id) {
            p.quantity += qty;
            fseek(pfp, -sizeof(Product), SEEK_CUR);
            fwrite(&p, sizeof(Product), 1, pfp);

            Shipment sh;
            sh.product_id = id;
            sh.quantity = qty;
            sh.total_cost = qty * p.cost_per_unit;
            strcpy(sh.timestamp, ctime(&(time_t){time(NULL)}));

            fwrite(&sh, sizeof(Shipment), 1, sfp);
            printf("Shipment recorded.\n");
            logActivity("Shipment recorded.");
            break;
        }
    }

    fclose(pfp);
    fclose(sfp);
}

/* ===================== ANALYTICS ===================== */

void costAnalytics() {
    FILE *fp = fopen("shipments.dat", "rb");
    if (!fp) return;

    Shipment s;
    float arr[500];
    int n = 0;

    while (fread(&s, sizeof(Shipment), 1, fp) && n < 500) {
        arr[n++] = s.total_cost;
    }
    fclose(fp);

    if (n == 0) {
        printf("No shipment data.\n");
        return;
    }

    float sum = 0;
    for (int i = 0; i < n; i++) sum += arr[i];
    float mean = sum / n;

    float variance = 0;
    for (int i = 0; i < n; i++)
        variance += pow(arr[i] - mean, 2);
    variance /= n;

    float std = sqrt(variance);

    printf("\n--- Shipment Cost Analytics ---\n");
    printf("Total Shipments: %d\n", n);
    printf("Average Cost: %.2f\n", mean);
    printf("Variance: %.2f\n", variance);
    printf("Standard Deviation: %.2f\n", std);

    logActivity("Cost analytics generated.");
}

/* ===================== MAIN ===================== */

int main() {
    initializeAdmin();

    printf("=== SUPPLY CHAIN SYSTEM ===\n");

    if (!login()) return 0;

    int choice;

    while (1) {
        printf("\n1.Add Product\n");
        printf("2.View Products\n");
        printf("3.Add Supplier\n");
        printf("4.Record Shipment\n");
        printf("5.Cost Analytics\n");
        printf("6.Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addProduct(); break;
            case 2: viewProducts(); break;
            case 3: addSupplier(); break;
            case 4: recordShipment(); break;
            case 5: costAnalytics(); break;
            case 6:
                logActivity("System exited.");
                exit(0);
            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}