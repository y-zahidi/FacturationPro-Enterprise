-- ============================================================================
-- FacturationPro Enterprise — database_schema.sql
-- Core table definitions for the SQLite database.
--
-- This is a CODE SNIPPET — shows the main table structures.
-- Sample data seeding and trigger logic are proprietary and not distributed.
-- ============================================================================

-- Enable foreign key enforcement
PRAGMA foreign_keys = ON;

-- ============================================================================
-- USERS — Authentication and role management
-- ============================================================================
CREATE TABLE IF NOT EXISTS Users (
    Id                   INTEGER PRIMARY KEY AUTOINCREMENT,
    Username             TEXT UNIQUE NOT NULL,
    Password             TEXT NOT NULL,
    FullName             TEXT NOT NULL,
    Email                TEXT UNIQUE,
    Role                 TEXT NOT NULL CHECK(Role IN ('ADMIN', 'USER')),
    IsActive             INTEGER DEFAULT 1,
    FailedLoginAttempts  INTEGER DEFAULT 0,
    LastLoginAt          DATETIME,
    LockedUntil          DATETIME,          -- NULL = not locked
    CreatedAt            DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt            DATETIME DEFAULT CURRENT_TIMESTAMP,
    IsDeleted            INTEGER DEFAULT 0  -- Soft delete
);

-- ============================================================================
-- CLIENTS — Customer records with credit tracking
-- ============================================================================
CREATE TABLE IF NOT EXISTS Clients (
    Id                   INTEGER PRIMARY KEY AUTOINCREMENT,
    Code                 TEXT UNIQUE,                          -- Auto: CLT001, CLT002...
    Name                 TEXT NOT NULL,
    Email                TEXT NOT NULL,
    Phone                TEXT NOT NULL,
    Address              TEXT,
    City                 TEXT,
    PostalCode           TEXT,
    TaxId                TEXT,
    CreditLimit          REAL DEFAULT 50000 CHECK(CreditLimit >= 0),
    TotalSpent           REAL DEFAULT 0     CHECK(TotalSpent >= 0),
    OutstandingBalance   REAL DEFAULT 0,
    Notes                TEXT,
    CreatedAt            DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt            DATETIME DEFAULT CURRENT_TIMESTAMP,
    IsDeleted            INTEGER DEFAULT 0
);

-- ============================================================================
-- PRODUCTS — Product catalog with stock management
-- ============================================================================
CREATE TABLE IF NOT EXISTS Products (
    Id                   INTEGER PRIMARY KEY AUTOINCREMENT,
    Code                 TEXT UNIQUE NOT NULL,                 -- Auto: PROD001...
    Name                 TEXT NOT NULL,
    Description          TEXT,
    Category             TEXT,                                 -- e.g. "Informatique"
    Price                REAL NOT NULL CHECK(Price > 0),       -- Selling price (EUR)
    CostPrice            REAL DEFAULT 0 CHECK(CostPrice >= 0), -- Purchase price
    Stock                INTEGER DEFAULT 0 CHECK(Stock >= 0),
    ReorderLevel         INTEGER DEFAULT 10 CHECK(ReorderLevel >= 0),
    Unit                 TEXT DEFAULT 'pcs',
    IsActive             INTEGER DEFAULT 1,
    CreatedAt            DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt            DATETIME DEFAULT CURRENT_TIMESTAMP,
    IsDeleted            INTEGER DEFAULT 0
);

-- ============================================================================
-- INVOICES — Main invoicing table (covers both invoices and quotes)
-- ============================================================================
CREATE TABLE IF NOT EXISTS Invoices (
    Id                   INTEGER PRIMARY KEY AUTOINCREMENT,
    Number               TEXT UNIQUE NOT NULL,                 -- FAC-YYYYMMDDnnnn
    ClientId             INTEGER NOT NULL,
    ClientName           TEXT NOT NULL,                        -- Denormalized for speed
    Date                 DATETIME NOT NULL,
    DueDate              DATETIME NOT NULL,
    Status               TEXT DEFAULT 'DRAFT'
                            CHECK(Status IN ('DRAFT','ISSUED','PAID','OVERDUE','CANCELLED')),
    Type                 TEXT DEFAULT 'INVOICE'
                            CHECK(Type IN ('INVOICE','QUOTE')),
    Subtotal             REAL DEFAULT 0 CHECK(Subtotal >= 0),
    DiscountPercent      REAL DEFAULT 0 CHECK(DiscountPercent >= 0 AND DiscountPercent <= 100),
    DiscountAmount       REAL DEFAULT 0 CHECK(DiscountAmount >= 0),
    TaxRate              REAL DEFAULT 5,                       -- TVA percentage
    TaxAmount            REAL DEFAULT 0 CHECK(TaxAmount >= 0),
    Total                REAL DEFAULT 0 CHECK(Total >= 0),     -- Final amount
    Notes                TEXT,
    CreatedAt            DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt            DATETIME DEFAULT CURRENT_TIMESTAMP,
    IsDeleted            INTEGER DEFAULT 0,

    FOREIGN KEY (ClientId) REFERENCES Clients(Id)
);

-- ============================================================================
-- INVOICE_LINES — Line items within an invoice
-- ============================================================================
CREATE TABLE IF NOT EXISTS InvoiceLines (
    Id                   INTEGER PRIMARY KEY AUTOINCREMENT,
    InvoiceId            INTEGER NOT NULL,
    ProductId            INTEGER NOT NULL,
    ProductCode          TEXT NOT NULL,                        -- Snapshot at creation
    ProductName          TEXT NOT NULL,                        -- Snapshot at creation
    Quantity             INTEGER NOT NULL CHECK(Quantity > 0),
    UnitPrice            REAL NOT NULL CHECK(UnitPrice > 0),   -- Price at time of sale
    Total                REAL NOT NULL CHECK(Total >= 0),      -- Qty × UnitPrice

    FOREIGN KEY (InvoiceId) REFERENCES Invoices(Id),
    FOREIGN KEY (ProductId) REFERENCES Products(Id)
);

-- ============================================================================
-- SETTINGS — App-wide configuration (singleton: always Id = 1)
-- ============================================================================
CREATE TABLE IF NOT EXISTS Settings (
    Id                   INTEGER PRIMARY KEY CHECK(Id = 1),   -- Only one row
    CompanyName          TEXT DEFAULT 'Ma Societe S.A.',
    CompanyAddress       TEXT DEFAULT '',
    CompanyCity          TEXT DEFAULT '',
    CompanyPhone         TEXT DEFAULT '',
    CompanyEmail         TEXT DEFAULT '',
    CompanyLogoPath      TEXT DEFAULT '',
    PDFColor             TEXT DEFAULT '#2D3E50',              -- Brand color for PDFs
    TaxRate              REAL DEFAULT 5,
    InvoiceTemplate      TEXT DEFAULT 'Modern'                -- Modern | Classic | Minimalist
);

-- ============================================================================
-- AUDIT_LOG — Action tracking (append-only, never deleted)
-- ============================================================================
CREATE TABLE IF NOT EXISTS AuditLog (
    Id                   INTEGER PRIMARY KEY AUTOINCREMENT,
    DateTime             DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    Username             TEXT NOT NULL,
    Action               TEXT NOT NULL,    -- LOGIN, SAVE_INVOICE, DELETE_CLIENT...
    Details              TEXT
);

-- [Additional logic: sample data seeding, index creation, default settings insert — not shown]
