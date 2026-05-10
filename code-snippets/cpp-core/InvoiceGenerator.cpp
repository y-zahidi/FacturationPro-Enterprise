// ============================================================================
// FacturationPro Enterprise — InvoiceGenerator.cpp
// Generates invoice/quote HTML from data, supports 3 templates.
//
// This is a CODE SNIPPET — partial extract for demonstration purposes.
// The full source is proprietary and not distributed.
// ============================================================================

#include "InvoiceGenerator.h"

// --- Utility: format a double as currency string (e.g. "15 792,00") ---
String InvoiceGenerator::FormatMoney(double amount) {
    return FormatFloat("#,##0.00", amount);
}

/**
 * GenerateHTML — Main entry point.
 *
 * Takes the invoice data, client info, company config, and a template type,
 * and returns a complete HTML document string ready for rendering or PDF export.
 *
 * Template selection happens here — each template gets its own CSS block
 * but shares the same data-binding logic below.
 */
String InvoiceGenerator::GenerateHTML(
    const InvoiceData& invoice,
    const ClientData& client,
    const CompanyInfo& company,
    InvoiceTemplate type)
{
    // Inject template-specific CSS
    String css = GetCssForTemplate(type, company.PrimaryColor);

    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'>";

    // Title changes based on document type
    String docTitle = (invoice.Type == "QUOTE") ? "Devis " : "Facture ";
    html += "<title>" + docTitle + invoice.Number + "</title>";
    html += "<style>" + css + "</style>";
    html += "</head><body>";

    // Container class differs for quotes (adds a dashed border)
    String containerClass = (invoice.Type == "QUOTE")
        ? "container quote-container"
        : "container";
    html += "<div class='" + containerClass + "'>";

    // --- LOGO HANDLING ---
    // If a company logo is configured and exists on disk, embed it
    String logoHtml = "";
    if (!company.LogoPath.IsEmpty() && FileExists(company.LogoPath)) {
        logoHtml = "<img src='file:///" +
            StringReplace(company.LogoPath, "\\", "/", TReplaceFlags() << rfReplaceAll) +
            "' style='max-height:80px;max-width:200px;object-fit:contain'>";
    }

    // --- MODERN TEMPLATE LAYOUT ---
    if (type == InvoiceTemplate::Modern) {
        // Header block: logo + invoice reference badge
        html += "<div class='header'>";
        html += "  <div class='logo'>";
        if (!logoHtml.IsEmpty()) html += logoHtml + "<br>";
        html += company.Name + "</div>";

        html += "  <div class='invoice-tag'>";
        String typeLabel = (invoice.Type == "QUOTE") ? "DEVIS" : "FACTURE";
        html += "    <div class='tag-label'>" + typeLabel + " REFERENCE</div>";
        html += "    <div class='tag-value'>" + invoice.Number + "</div>";
        html += "  </div>";
        html += "</div>";

        // Meta grid: From / To / Details columns
        html += "<div class='meta-grid'>";

        // "From" column — company info
        html += "  <div class='meta-col'>";
        html += "    <div class='meta-label'>De</div>";
        html += "    <div class='meta-value'><strong>" + company.Name + "</strong><br>"
                + company.Address + "<br>" + company.City + "<br>"
                + company.Email + "</div>";
        html += "  </div>";

        // "To" column — client info
        html += "  <div class='meta-col'>";
        String clientLabel = (invoice.Type == "QUOTE") ? "Adresser a" : "Pour";
        html += "    <div class='meta-label'>" + clientLabel + "</div>";
        html += "    <div class='meta-value'><strong>" + client.Name + "</strong><br>"
                + client.Address + "<br>" + client.City + "</div>";
        html += "  </div>";

        // "Details" column — dates
        html += "  <div class='meta-col' style='text-align:right'>";
        html += "    <div class='meta-label'>Details</div>";
        String dateLabel = (invoice.Type == "QUOTE") ? "Valable jusqu'au: " : "Echeance: ";
        html += "    <div class='meta-value'>Date: " + invoice.Date + "<br>"
                + dateLabel + invoice.DueDate + "</div>";
        html += "  </div>";

        html += "</div>"; // end meta-grid

        // Status badge — only show for non-DRAFT documents
        if (invoice.Status != "DRAFT") {
            html += "<div class='status-badge status-" + invoice.Status.ToLower() + "'>"
                    + invoice.Status + "</div>";
        }
    }
    // ... (Classic and Minimalist template layouts follow the same pattern)
    // ... (rest of template rendering not shown)

    // --- LINE ITEMS TABLE ---
    // Shared across all templates — the product grid
    html += "<table class='items-table'>";
    html += "  <thead><tr>";
    html += "    <th>Code</th><th>Produit</th><th>Qte</th><th>Prix Unit.</th><th>Total</th>";
    html += "  </tr></thead>";
    html += "  <tbody>";

    for (int i = 0; i < invoice.Lines.Count; i++) {
        const InvoiceLineData& line = invoice.Lines[i];
        html += "    <tr>";
        html += "      <td>" + line.ProductCode + "</td>";
        html += "      <td>" + line.ProductName + "</td>";
        html += "      <td>" + IntToStr(line.Quantity) + "</td>";
        html += "      <td>" + FormatMoney(line.UnitPrice) + "</td>";
        html += "      <td class='amount'>" + FormatMoney(line.Total) + "</td>";
        html += "    </tr>";
    }

    html += "  </tbody>";
    html += "</table>";

    // --- TOTALS BLOCK ---
    html += "<div class='totals'>";
    html += "  <div class='total-row'><span>Sous-total</span><span>"
            + FormatMoney(invoice.Subtotal) + " EUR</span></div>";

    // Only show discount row if a discount is applied
    if (invoice.DiscountAmount > 0) {
        html += "  <div class='total-row discount'><span>Remise</span><span>-"
                + FormatMoney(invoice.DiscountAmount) + " EUR</span></div>";
    }

    html += "  <div class='total-row'><span>TVA " + FormatFloat("0.0", invoice.TaxRate)
            + "%</span><span>" + FormatMoney(invoice.TaxAmount) + " EUR</span></div>";
    html += "  <div class='total-row total'><span>TOTAL</span><span>"
            + FormatMoney(invoice.Total) + " EUR</span></div>";
    html += "</div>";

    // ... (footer with payment terms, notes, and thank-you message not shown)

    html += "</div>"; // end container
    html += "</body></html>";

    return html;
}

// ... (GetCssForTemplate implementation — returns ~200 lines of CSS per template — not shown)
