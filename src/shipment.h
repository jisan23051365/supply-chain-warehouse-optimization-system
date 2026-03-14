/*
 * shipment.h - Shipment tracking declarations
 * Supply Chain & Warehouse Management System
 */

#ifndef SHIPMENT_H
#define SHIPMENT_H

#include "common.h"

/* Allowed status values */
#define STATUS_PENDING    "Pending"
#define STATUS_IN_TRANSIT "In-Transit"
#define STATUS_DELIVERED  "Delivered"
#define STATUS_CANCELLED  "Cancelled"

/* ── Shipment record ─────────────────────────────────────────────────── */
typedef struct {
    int   id;
    int   product_id;
    int   supplier_id;
    int   quantity;
    char  shipment_date[MAX_DATE_LEN];
    char  expected_arrival[MAX_DATE_LEN];
    char  status[MAX_STATUS_LEN];
    float shipping_cost;
} Shipment;

/* ── CRUD interface ──────────────────────────────────────────────────── */
void create_shipment(void);
void view_all_shipments(void);
void track_shipment(void);
void update_shipment_status(void);
void delete_shipment(void);

/* ── File I/O ────────────────────────────────────────────────────────── */
int load_shipments(Shipment shipments[], int *count);
int save_shipments(const Shipment shipments[], int count);

/* ── Sub-menu entry point ────────────────────────────────────────────── */
void shipment_menu(void);

#endif /* SHIPMENT_H */
