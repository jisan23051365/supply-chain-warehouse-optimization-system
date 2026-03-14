/*
 * analytics.h - Logistics cost analytics declarations
 * Supply Chain & Warehouse Management System
 */

#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "common.h"
#include "shipment.h"
#include "supplier.h"
#include "product.h"

/* ── Analytics reports ───────────────────────────────────────────────── */
void report_total_cost(void);
void report_cost_by_supplier(void);
void report_cost_by_product(void);
void report_shipment_summary(void);

/* ── Sub-menu entry point ────────────────────────────────────────────── */
void analytics_menu(void);

#endif /* ANALYTICS_H */
