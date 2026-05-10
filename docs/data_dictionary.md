# Data Dictionary — FacturationPro Enterprise

This document describes every table in the SQLite database, their columns, constraints, and how they relate to each other.

---

## Entity relationship summary

```
Users ──────────────────► AuditLog
  │
  └── (current session)

Clients ────────────────► Invoices ──────────► InvoiceLines ◄──── Products
  │                          │
  └── (credit tracking)      └── (status workflow)

Settings (singleton row — app-wide config)
```

---

## Tables

### Users

Manages application users and authentication state.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| Id | INTEGER | PK, AUTOINCREMENT | Unique user identifier |
| Username | TEXT | UNIQUE, NOT NULL | Login name |
| Password | TEXT | NOT NULL | Password (hashed in production builds) |
| FullName | TEXT | NOT NULL | Display name |
| Email | TEXT | UNIQUE | Contact email |
| Role | TEXT | CHECK (ADMIN, USER) | Permission level |
| IsActive | INTEGER | DEFAULT 1 | Account enabled flag |
| FailedLoginAttempts | INTEGER | DEFAULT 0 | Tracks consecutive failures |
| LastLoginAt | DATETIME | — | Timestamp of most recent login |
| LockedUntil | DATETIME | — | Account lockout expiry (NULL if not locked) |
| CreatedAt | DATETIME | DEFAULT CURRENT_TIMESTAMP | Creation timestamp |
| UpdatedAt | DATETIME | DEFAULT CURRENT_TIMESTAMP | Last modification timestamp |
| IsDeleted | INTEGER | DEFAULT 0 | Soft delete flag |

**Notes:** Account lockout triggers after 5 consecutive failed login attempts. The lockout duration is 15 minutes. `IsDeleted` is used everywhere — no hard deletes in the system.

---

### Clients

Customer records with credit management fields.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| Id | INTEGER | PK, AUTOINCREMENT | Unique client identifier |
| Code | TEXT | UNIQUE | Auto-generated code (CLT001, CLT002...) |
| Name | TEXT | NOT NULL | Company or individual name |
| Email | TEXT | NOT NULL | Contact email |
| Phone | TEXT | NOT NULL | Phone number |
| Address | TEXT | — | Street address |
| City | TEXT | — | City |
| PostalCode | TEXT | — | Postal code |
| TaxId | TEXT | — | Tax identification number |
| CreditLimit | REAL | DEFAULT 50000, CHECK ≥ 0 | Maximum allowed outstanding balance |
| TotalSpent | REAL | DEFAULT 0, CHECK ≥ 0 | Lifetime total invoiced amount |
| OutstandingBalance | REAL | DEFAULT 0 | Current unpaid balance |
| Notes | TEXT | — | Free-text notes |
| CreatedAt | DATETIME | DEFAULT CURRENT_TIMESTAMP | — |
| UpdatedAt | DATETIME | DEFAULT CURRENT_TIMESTAMP | — |
| IsDeleted | INTEGER | DEFAULT 0 | Soft delete flag |

**Notes:** `TotalSpent` and `OutstandingBalance` are updated automatically by the DataManager when invoices change status. The `CreditLimit` field is intended for future use in blocking invoice creation when a client exceeds their limit.

---

### Products

Product catalog with stock and pricing.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| Id | INTEGER | PK, AUTOINCREMENT | Unique product identifier |
| Code | TEXT | UNIQUE, NOT NULL | Auto-generated code (PROD001, PROD002...) |
| Name | TEXT | NOT NULL | Product name |
| Description | TEXT | — | Detailed description |
| Category | TEXT | — | Product category (e.g., "Informatique") |
| Price | REAL | NOT NULL, CHECK > 0 | Selling price in EUR |
| CostPrice | REAL | DEFAULT 0, CHECK ≥ 0 | Purchase/cost price (for margin tracking) |
| Stock | INTEGER | DEFAULT 0, CHECK ≥ 0 | Current stock quantity |
| ReorderLevel | INTEGER | DEFAULT 10, CHECK ≥ 0 | Threshold for low-stock alerts |
| Unit | TEXT | DEFAULT 'pcs' | Unit of measure |
| IsActive | INTEGER | DEFAULT 1 | Product active flag |
| CreatedAt | DATETIME | DEFAULT CURRENT_TIMESTAMP | — |
| UpdatedAt | DATETIME | DEFAULT CURRENT_TIMESTAMP | — |
| IsDeleted | INTEGER | DEFAULT 0 | Soft delete flag |

**Notes:** Stock is decremented automatically when an invoice containing this product is saved. The `ReorderLevel` field will be used for dashboard alerts in v2.2.

---

### Invoices

The main invoicing table — covers both invoices and quotes.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| Id | INTEGER | PK, AUTOINCREMENT | Unique invoice identifier |
| Number | TEXT | UNIQUE, NOT NULL | Invoice reference (FAC-202601312317) |
| ClientId | INTEGER | FK → Clients.Id | Associated client |
| ClientName | TEXT | NOT NULL | Denormalized client name (for display speed) |
| Date | DATETIME | NOT NULL | Invoice/quote date |
| DueDate | DATETIME | NOT NULL | Payment due date or quote expiry |
| Status | TEXT | DEFAULT 'DRAFT', CHECK | Workflow state (see below) |
| Type | TEXT | DEFAULT 'INVOICE', CHECK | INVOICE or QUOTE |
| Subtotal | REAL | DEFAULT 0, CHECK ≥ 0 | Sum of line totals before tax |
| DiscountPercent | REAL | DEFAULT 0 | Global discount percentage |
| DiscountAmount | REAL | DEFAULT 0 | Calculated discount amount |
| TaxRate | REAL | DEFAULT 5 | TVA rate (%) |
| TaxAmount | REAL | DEFAULT 0 | Calculated tax amount |
| Total | REAL | DEFAULT 0 | Final total (subtotal - discount + tax) |
| Notes | TEXT | — | Additional notes printed on the invoice |
| CreatedAt | DATETIME | DEFAULT CURRENT_TIMESTAMP | — |
| UpdatedAt | DATETIME | DEFAULT CURRENT_TIMESTAMP | — |
| IsDeleted | INTEGER | DEFAULT 0 | Soft delete flag |

**Status workflow:**
```
DRAFT → ISSUED → PAID
                ↘ OVERDUE
         ↘ CANCELLED
```

**Notes:** The `Number` field follows the format `FAC-YYYYMMDDnnnn` for invoices and `DEV-YYYYMMDDnnnn` for quotes, auto-generated by DataManager. `ClientName` is stored redundantly for performance — the dashboard and list views don't need to JOIN every time.

---

### InvoiceLines

Line items within an invoice.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| Id | INTEGER | PK, AUTOINCREMENT | Unique line identifier |
| InvoiceId | INTEGER | FK → Invoices.Id | Parent invoice |
| ProductId | INTEGER | FK → Products.Id | Associated product |
| ProductCode | TEXT | NOT NULL | Denormalized product code |
| ProductName | TEXT | NOT NULL | Denormalized product name |
| Quantity | INTEGER | NOT NULL, CHECK > 0 | Quantity ordered |
| UnitPrice | REAL | NOT NULL, CHECK > 0 | Unit price at time of sale |
| Total | REAL | NOT NULL, CHECK ≥ 0 | Quantity × UnitPrice |

**Notes:** `UnitPrice` is captured at the time the invoice is created — it does NOT change if the product price is updated later. This is intentional and important for accurate historical records.

---

### Settings

Application-wide configuration stored as a single row.

| Column | Type | Description |
|--------|------|-------------|
| Id | INTEGER | Always 1 (singleton) |
| CompanyName | TEXT | Company name shown on invoices and dashboard |
| CompanyAddress | TEXT | Street address |
| CompanyCity | TEXT | City |
| CompanyPhone | TEXT | Phone number |
| CompanyEmail | TEXT | Email |
| CompanyLogoPath | TEXT | File path to company logo |
| PDFColor | TEXT | Primary color for PDF invoices (hex, e.g., #2D3E50) |
| TaxRate | REAL | Default TVA rate applied to new invoices |
| InvoiceTemplate | TEXT | Active template name (Modern, Classic, Minimalist) |

---

### AuditLog

Records all significant actions in the system.

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| Id | INTEGER | PK, AUTOINCREMENT | Log entry identifier |
| DateTime | DATETIME | NOT NULL | When the action occurred |
| Username | TEXT | NOT NULL | Who did it |
| Action | TEXT | NOT NULL | Action type (LOGIN, SAVE_INVOICE, DELETE_CLIENT...) |
| Details | TEXT | — | Additional context |

**Notes:** This table grows over time and is append-only. No entries are ever deleted or soft-deleted. It's meant for admin review and debugging, not for high-volume analytics.

---

## Sample data

On first launch, the database is pre-populated with:

- **2 users** — `admin` (ADMIN) and `user` (USER)
- **50 clients** — Realistic Moroccan company names with proper codes
- **49 products** — IT equipment and services with stock levels
- **100+ invoices** — Mix of invoices and quotes across multiple months, with line items

This gives the dashboard and reports meaningful data to display right away.
