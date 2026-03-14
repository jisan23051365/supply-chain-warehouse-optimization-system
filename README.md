# Supply Chain & Warehouse Optimization System

## Overview
The Supply Chain & Warehouse Optimization System is a console-based logistics management application written in C. The system simulates warehouse operations including product inventory management, supplier tracking, shipment recording, and logistics cost analytics.

The system also includes authentication, activity logging, and file-based storage to maintain supply chain records.

## Features

### Authentication System
- Admin login with username and password
- Password hashing for security
- Automatic administrator initialization

### Product Inventory Management
- Add products to warehouse inventory
- View product inventory records
- Track product quantities and cost per unit
- Automatic reorder alerts when stock falls below reorder level

### Supplier Management
- Add supplier information
- Maintain supplier reliability scores

### Shipment Management
- Record incoming shipments
- Automatically update product inventory
- Track shipment quantity and cost
- Store shipment records with timestamps

### Cost Analytics
The system analyzes shipment data and provides statistical insights including:

- Total shipments recorded
- Average shipment cost
- Variance
- Standard deviation

### Activity Logging
All system operations are logged including:

- Admin login
- Product additions
- Supplier registration
- Shipment recording
- Analytics generation
- System exit

## Technologies Used
- C Programming Language
- Standard C Libraries
- File-based persistent storage

## System Data Files

| File | Purpose |
|-----|-----|
| supply_users.dat | Authentication records |
| products.dat | Warehouse product inventory |
| suppliers.dat | Supplier information |
| shipments.dat | Shipment transaction records |
| supply_logs.txt | System activity logs |

## Default Admin Login

Username: admin  
Password: admin123

## Program Menu

1. Add Product  
2. View Products  
3. Add Supplier  
4. Record Shipment  
5. Cost Analytics  
6. Exit  

## Learning Objectives

This project demonstrates:

- Supply chain management simulation
- Inventory control systems
- Logistics tracking
- File handling in C
- Authentication using hashing
- Statistical analytics
- Console-based warehouse management systems

## Author
Jisan
