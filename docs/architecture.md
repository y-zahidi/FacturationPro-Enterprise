# Architecture — FacturationPro Enterprise

This document explains how the application is structured internally. It's meant to give you a clear mental model of the codebase without needing to read every file.

---

## Layer overview

The application follows a relatively clean layered architecture, even though it's a single desktop app:

```
┌─────────────────────────────────────────────────┐
│              UI Layer (Forms + Custom Components) │
│   MainForm, DashboardForm, InvoiceListForm...    │
│   ModernButton, ModernKPICard, ModernComponent   │
├─────────────────────────────────────────────────┤
│              Service Layer (DataManager)          │
│   Business logic, orchestration, audit logging   │
├─────────────────────────────────────────────────┤
│              DAO Layer (Data Access Objects)      │
│   ClientDAO, ProductDAO, InvoiceDAO, UserDAO     │
│   BaseDAO<T> generic template                    │
├─────────────────────────────────────────────────┤
│              Connection Layer (DBConnection)      │
│   Singleton, FireDAC, connection pooling         │
├─────────────────────────────────────────────────┤
│              Database (SQLite)                    │
│   facturation.db — 7 tables, FK enforced         │
└─────────────────────────────────────────────────┘
```

---

## Connection layer — DBConnection

`DBConnection` is a singleton that owns the single `TFDConnection` to the SQLite database. It handles:

- Lazy initialization (connection is only created when first accessed)
- Connection pooling (up to 50 items, configured via FireDAC params)
- Foreign key enforcement (`PRAGMA foreign_keys = ON`)
- Transaction support — `beginTransaction()`, `commit()`, `rollback()`
- Directory creation if the database folder doesn't exist yet

There is exactly one instance of this class at any point in the application lifecycle. Every DAO and the DataManager get their connection through `DBConnection::getInstance()`.

---

## DAO layer — BaseDAO<T> and typed DAOs

The DAO (Data Access Object) pattern is used to isolate all SQL from the rest of the application. Each entity type has its own DAO class that inherits from a generic `BaseDAO<T>` template.

`BaseDAO<T>` provides the shared plumbing — query execution, error handling, and the contract that every DAO must fulfill:

- `mapRow(query)` — map a database row into an entity object
- `setInsertParams(query, entity)` — bind parameters for INSERT
- `setUpdateParams(query, entity)` — bind parameters for UPDATE

So `ClientDAO` just implements those three methods plus any client-specific queries (like searching by name or getting credit status). The pattern means adding a new entity type is fast — write the model struct, write the mapping, and you're done.

---

## Service layer — DataManager

`DataManager` is the main service object. It's a global singleton (`extern TDataManager* DataManager`) that the UI layer calls for everything.

It owns the database connection and exposes high-level operations:

- **Auth** — `Login()` with credential validation and account lockout after failed attempts
- **CRUD** — `GetClients()`, `SaveInvoice()`, `DeleteProduct()`, etc.
- **Business logic** — invoice number generation, quote-to-invoice conversion, stock updates when an invoice is saved
- **Reporting** — `GetMonthlyRevenue()`, `GetTopClients()`, `GetTopProducts()` — all the data the dashboard and reports screens need
- **Maintenance** — `BackupDatabase()` and `RestoreDatabase()` with automatic timestamp-based file naming
- **Audit** — `LogAction()` records every significant operation with the current user and a timestamp

The DataManager is the only layer that knows about business rules. DAOs are purely about data access; the UI is purely about display. DataManager is the glue in between.

---

## UI layer — Forms and custom components

### Forms

Each screen in the application is a Delphi Form (`TForm` subclass). The main ones:

| Form | Role |
|------|------|
| `LoginForm` | Authentication screen with modern button styling |
| `MainForm` | The main application shell — sidebar navigation, tab switching |
| `DashboardForm` | KPI cards, revenue chart, recent invoices, top clients |
| `ClientListForm` / `ClientEditForm` | Client list with search + inline editing |
| `ProductListForm` / `ProductEditForm` | Product catalog management |
| `InvoiceListForm` / `InvoiceEditForm` | Invoice creation and editing with line items |
| `InvoicePreviewForm` | HTML preview of the generated invoice before PDF export |

### Custom UI components

The application uses a small hand-rolled component library to achieve a modern, SaaS-style look without third-party UI libraries:

**`ModernComponent`** — Base class. Handles border radius, elevation (shadow), and hover states by overriding the paint event.

**`ModernButton`** — Styled button with three variants: Primary (solid green), Ghost (outline), and Danger (red). Supports hover color transitions.

**`ModernKPICard`** — The dashboard stat cards. Each one shows an icon, a title, a large value, and a colored trend indicator (+5.0%, -2.1%, etc.). The accent bar on the left changes color based on the card type.

### Design system — ModernColors

All colors in the application are defined in `ModernColors.h` as a namespace of `const TColor` values. This includes:

- Primary palette (the green used in the sidebar and buttons)
- Accent, Success, Warning, Danger colors
- A full neutral gray scale (50 through 900)
- Background colors for primary, secondary, and tertiary surfaces
- A dark mode palette (defined but not yet wired up — planned for v2.2)

This centralization means changing the app's color scheme is a single-file edit.

---

## Invoice PDF generation — InvoiceGenerator

PDF export works by generating an HTML document in memory, rendering it to a browser control, and then capturing the output as PDF. The `InvoiceGenerator` class handles the HTML construction.

Three templates are supported, selectable from the Settings panel:

- **Modern** — Clean SaaS style with Inter font, colored badges, and a structured grid layout
- **Classic** — More formal, Times New Roman, traditional border-based layout
- **Minimalist** — Black and white, ink-saver friendly, simple structure

The company logo (if configured) is embedded by reading the file from disk and inserting it as a `file:///` path in the HTML. The primary brand color is also injected into the CSS dynamically.

---

## Database initialization

On first launch, if no database file exists, `DataManager::Initialize()` runs the schema script (`database_schema.sql`) to create all tables, then loads sample data (50 clients, 49 products, 100+ invoices). This means the application is usable immediately after installation — no setup wizard, no empty screens.

On subsequent launches, it simply connects to the existing database and skips initialization.
