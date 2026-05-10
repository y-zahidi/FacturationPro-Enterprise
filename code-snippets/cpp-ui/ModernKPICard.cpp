// ============================================================================
// FacturationPro Enterprise — ModernKPICard.cpp
// Custom UI component: the stat cards shown on the dashboard.
//
// Each card displays: an icon, a title, a large value, and a trend indicator.
// The left accent bar color changes based on the card type (clients, products,
// invoices, revenue).
//
// This is a CODE SNIPPET — partial extract for demonstration purposes.
// The full source is proprietary and not distributed.
// ============================================================================

#include "ModernKPICard.h"
#include <algorithm>

/**
 * Constructor — builds the card's internal layout.
 *
 * The card is composed of several TLabel and TPanel children, all positioned
 * absolutely within the card's bounds. No layout manager — manual pixel
 * positioning gives full control over the modern look.
 *
 * Layout (top to bottom):
 *   [Accent Bar (4px left edge)]
 *   [Icon — large emoji, top-left]
 *   [Title — small gray text]
 *   [Value — large bold number]
 *   [Trend — colored percentage change]
 */
__fastcall TModernKPICard::TModernKPICard(TComponent *Owner)
    : TModernComponent(Owner)
{
    Width  = 280;
    Height = 160;
    SetBorderRadius(12);   // Rounded corners via base class
    SetElevation(1);       // Subtle shadow
    SetHoverable(true);    // Slight lift on hover

    // --- Accent bar: 4px colored strip on the left edge ---
    FAccentBar = new TPanel(this);
    FAccentBar->Parent  = this;
    FAccentBar->Left    = 0;
    FAccentBar->Top     = 0;
    FAccentBar->Width   = 4;
    FAccentBar->Height  = Height;
    FAccentBar->Align   = alLeft;
    FAccentBar->BevelOuter = bvNone;
    FAccentBar->Color   = ModernColors::PRIMARY_500;  // Default green

    // --- Icon: large emoji rendered via Segoe UI Emoji font ---
    FIconLabel = new TLabel(this);
    FIconLabel->Parent     = this;
    FIconLabel->Left       = 20;
    FIconLabel->Top        = 20;
    FIconLabel->Font->Size = 32;
    FIconLabel->Font->Name = "Segoe UI Emoji";
    FIconLabel->AutoSize   = true;
    FIconLabel->Transparent = true;
    FIconLabel->Caption    = "📊";  // Default; overridden by SetIcon()

    // --- Title: small uppercase label above the value ---
    FTitleLabel = new TLabel(this);
    FTitleLabel->Parent      = this;
    FTitleLabel->Left        = 20;
    FTitleLabel->Top         = 70;
    FTitleLabel->Font->Size  = 12;
    FTitleLabel->Font->Color = ModernColors::GRAY_500;
    FTitleLabel->Font->Name  = "Inter";
    FTitleLabel->AutoSize    = true;
    FTitleLabel->Transparent = true;
    FTitleLabel->Caption     = "TITRE";

    // --- Value: the main large number ---
    FValueLabel = new TLabel(this);
    FValueLabel->Parent      = this;
    FValueLabel->Left        = 20;
    FValueLabel->Top         = 90;
    FValueLabel->Font->Size  = 28;
    FValueLabel->Font->Style = TFontStyles() << fsBold;
    FValueLabel->Font->Color = ModernColors::GRAY_900;
    FValueLabel->Font->Name  = "Inter";
    FValueLabel->AutoSize    = true;
    FValueLabel->Transparent = true;
    FValueLabel->Caption     = "0";

    // --- Trend indicator: colored percentage change at the bottom ---
    FTrendLabel = new TLabel(this);
    FTrendLabel->Parent      = this;
    FTrendLabel->Left        = 20;
    FTrendLabel->Top         = 130;
    FTrendLabel->Font->Size  = 11;
    FTrendLabel->Font->Style = TFontStyles() << fsBold;
    FTrendLabel->Font->Color = ModernColors::SUCCESS_500;  // Green by default
    FTrendLabel->Font->Name  = "Inter";
    FTrendLabel->AutoSize    = true;
    FTrendLabel->Transparent = true;
    FTrendLabel->Caption     = "+0.0%";
}

/**
 * SetValue — Update the displayed value and optionally the trend.
 *
 * @param value   The main number to display (e.g., "50" or "73,806 DH")
 * @param trend   The percentage change string (e.g., "+5.0%" or "-2.1%")
 *
 * Trend color logic:
 *   - Positive (starts with "+") → green (SUCCESS_500)
 *   - Negative (starts with "-") → red (DANGER_500)
 *   - Zero or neutral            → gray (GRAY_400)
 */
void TModernKPICard::SetValue(const String& value, const String& trend) {
    FValueLabel->Caption = value;

    if (!trend.IsEmpty()) {
        FTrendLabel->Caption = trend;

        // Color the trend based on sign
        if (trend[0] == '+') {
            FTrendLabel->Font->Color = ModernColors::SUCCESS_500;
        } else if (trend[0] == '-') {
            FTrendLabel->Font->Color = ModernColors::DANGER_500;
        } else {
            FTrendLabel->Font->Color = ModernColors::GRAY_400;
        }
    }
}

/**
 * SetAccentColor — Change the left bar color.
 * Each card type uses a different accent:
 *   Clients   → PRIMARY_500 (green)
 *   Products  → ACCENT_500  (violet)
 *   Invoices  → WARNING_500 (amber)
 *   Revenue   → SUCCESS_500 (emerald)
 */
void TModernKPICard::SetAccentColor(TColor color) {
    FAccentBar->Color = color;
}

// ... (SetIcon, SetTitle, Paint override with shadow/border-radius — not shown)
// ... (Hover animation logic inherited from ModernComponent — not shown)
