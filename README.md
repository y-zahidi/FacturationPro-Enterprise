


# FacturationPro Enterprise

> **A complete enterprise invoicing & billing application built with C++ / Delphi, powered by SQLite and FireDAC.**

![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Delphi-darkgreen)
![Database](https://img.shields.io/badge/Database-SQLite%20via%20FireDAC-orange)
![Version](https://img.shields.io/badge/Version-2.1-purple)
![License](https://img.shields.io/badge/License-Proprietary-red)
![RAD Studio](https://img.shields.io/badge/RAD%20Studio-VCL-success)
![Status](https://img.shields.io/badge/Status-Stable-brightgreen)
[![CI](https://github.com/y-zahidi/FacturationPro-Enterprise/actions/workflows/ci.yml/badge.svg)](https://github.com/y-zahidi/FacturationPro-Enterprise/actions/workflows/ci.yml)

---

## What is FacturationPro Enterprise?

FacturationPro Enterprise is a full-featured desktop invoicing system designed for small and medium businesses. It handles everything from client and product management to invoice generation, PDF export, and business analytics — all in one clean, modern desktop application.

I built this because existing invoicing tools were either way too complex for what small businesses actually need, or too basic to handle real workflows like quote-to-invoice conversion, stock tracking, and multi-user access. The goal was something that just works, out of the box, with no server setup, no cloud dependency — just install and go.

Developed during the 2025-2026 academic year for educational purposes.

---

## What it does — at a glance

The application covers the full invoicing lifecycle. You log in, manage your clients and product catalog, create quotes, convert them to invoices when the deal is done, export professional PDFs, and track everything through a dashboard with real-time KPIs and revenue charts.

Here's the dashboard right when you open it — 50 clients, 49 products, 106 invoices tracked, with monthly revenue evolution and top-client rankings all visible at once:

![Dashboard](screenshots/dashboard.png)

---

## Screenshots

| Screen | Description |
|--------|-------------|
| ![Login](screenshots/login.png) | Clean login screen with role-based access (ADMIN / USER) |
| ![Dashboard](screenshots/dashboard.png) | Main dashboard — KPIs, revenue chart, recent invoices, top clients |
| ![Clients](screenshots/clients.png) | Client management — search, CRUD, export to CSV |
| ![Products](screenshots/products.png) | Product catalog with stock levels, categories, and pricing |
| ![Invoices](screenshots/invoices.png) | Invoice editor — line items, auto-calculated totals with TVA |
| ![Quotes](screenshots/quotes.png) | Quote management with validity dates and draft status |
| ![Reports](screenshots/reports.png) | Annual reports — revenue evolution, Top 5 clients & products |
| ![Settings](screenshots/settings.png) | Company settings, PDF theming, user management, backup/restore |

---

## Technical architecture

The application is structured around a few clear layers, and I kept it deliberately straightforward — no over-engineering.

**Core stack:** C++ with Delphi VCL components, SQLite as the database engine accessed through Embarcadero's FireDAC ORM, and a custom UI layer built on top of VCL panels and labels to get a modern SaaS look without third-party UI libraries.

**Database layer:** A singleton `DBConnection` class manages the SQLite connection with pooling enabled. All data access goes through typed DAO classes (`ClientDAO`, `ProductDAO`, `InvoiceDAO`) that inherit from a generic `BaseDAO<T>` template — so adding a new entity type is basically just writing the mapping and the SQL. The schema uses proper foreign keys, CHECK constraints, and soft-delete flags.

**The DataManager** acts as the central service layer. It owns the connection, exposes all business operations (CRUD for clients/products/invoices, login with lockout, audit logging, backup/restore), and handles things like auto-generating invoice numbers and stock updates on sale.

**Invoice PDF generation** is done by building HTML templates in memory and rendering them. There are three invoice templates (Modern, Classic, Minimalist) that can be switched from the settings panel. The company logo gets embedded directly into the PDF output.

**UI system:** I wrote a small custom component library (`ModernButton`, `ModernKPICard`, `ModernComponent`) that draws rounded corners, shadows, hover effects, and accent bars — basically a design system on top of raw VCL. All colors are defined in a central `ModernColors` namespace, which makes theming easy and consistent.

See [docs/architecture.md](docs/architecture.md) for a deeper breakdown.

---

## Key features

**Client & Product Management** — Full CRUD with search, CSV import/export, auto-generated codes (CLT001, PROD001...), and paginated lists.

**Invoicing & Quoting** — Create quotes, convert them to invoices in one click, add/remove line items with auto-calculated subtotals, TVA (5%), and totals. Support for DRAFT → ISSUED → PAID → OVERDUE status workflow.

**PDF Export** — Three professional invoice templates. Company logo, custom brand color, and all details rendered cleanly. HTML preview before export.

**Dashboard & Reporting** — Monthly revenue charts, Top 5 clients by spend, Top 5 products by volume, year-over-year filtering, and CSV export of report data.

**Multi-User Access** — Role-based system (ADMIN vs USER). Login with account lockout after failed attempts. Audit log tracks all actions.

**Data Maintenance** — One-click database backup and restore. The app handles the file paths and timestamps automatically.

---

## Database overview

The database has 7 core tables: `Users`, `Clients`, `Products`, `Invoices`, `InvoiceLines`, `Settings`, and `AuditLog`. Everything runs on SQLite — no external database server needed. Foreign keys are enforced, and all tables use soft deletes (`IsDeleted` flag) so nothing is ever truly lost.

With the sample data loaded, you're working with 50 clients, 49 products, and 100+ invoices right away — enough to see the charts and reports actually do something meaningful.

See [docs/data_dictionary.md](docs/data_dictionary.md) for the full table breakdown.

---

## Code snippets

The `code-snippets/` folder contains partial extracts from the actual codebase. These show the patterns and architecture decisions — not the full implementation. Each file has comments marking where code continues.

| Snippet | What it shows |
|---------|---------------|
| `cpp-core/DBConnection.h` | Singleton database connection with FireDAC, pooling, and transaction support |
| `cpp-core/InvoiceGenerator.cpp` | HTML invoice template rendering with multi-template support |
| `cpp-dao/ClientDAO.h` | Generic DAO pattern — how entity mapping and SQL queries are structured |
| `cpp-ui/ModernKPICard.cpp` | Custom UI component — the KPI cards on the dashboard |
| `cpp-ui/ModernColors.h` | The full design system color palette |
| `sql/database_schema.sql` | Core table definitions (partial) |

---

## Why this is proprietary

This isn't a "keep it to myself" situation — it's a deliberate choice. The application has real commercial potential as a standalone product for the SMB market, and the architecture and UI work here represent a significant chunk of my portfolio. Sharing the full source would dilute both the product value and the portfolio differentiation.

That said, the code snippets and architecture docs here are meant to show *how* things work. If you're interested in the project for a specific reason, feel free to reach out.

---

## What's next — the roadmap

This is v2.1 and it's far from done. Here's what I'm working on or planning:

**v2.2 — Coming soon**
- Dark mode (the color system already supports it, just need the UI toggle)
- Editable inline grid cells on the invoice editor
- Smarter stock alerts — low-stock warnings on the dashboard

**v3.0 — Planned**
- Migration to a client-server model (PostgreSQL backend)
- REST API layer for potential web frontend or mobile integration
- Machine learning for revenue forecasting based on historical patterns
- Multi-currency support
- Advanced discount rules (percentage, fixed, combo)

**Long-term**
- Cloud deployment option (SaaS version)
- Plugin system for custom report templates
- Integration with popular accounting software exports (QuickBooks, Sage)

---

## Built with

- **C++ / Delphi** — Embarcadero RAD Studio, VCL framework
- **SQLite** — Lightweight, file-based, zero configuration
- **FireDAC** — Embarcadero's ORM and database abstraction layer
- **Custom UI kit** — ModernButton, ModernKPICard, ModernComponent (hand-rolled on VCL)


---

⚠️ **This is NOT an open-source project.** No source code is distributed here — only code snippets are shown for portfolio and demonstration purposes. The full application is proprietary and under active development. See [LICENSE](LICENSE) for details.

