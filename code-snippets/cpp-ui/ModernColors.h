// ============================================================================
// FacturationPro Enterprise — ModernColors.h
// Central design system color palette.
//
// All UI colors in the application are defined here. Changing the app's
// visual theme is a matter of editing this single file.
//
// This is a CODE SNIPPET — the complete color definitions are shown.
// UI component rendering logic is proprietary and not distributed.
// ============================================================================

#ifndef ModernColorsH
#define ModernColorsH

#include <Vcl.Graphics.hpp>

namespace ModernColors {

// ---------------------------------------------------------------------------
// PRIMARY — The app's signature green (sidebar, buttons, accent bars)
// ---------------------------------------------------------------------------
const TColor PRIMARY_50  = (TColor)0xFFF9F0;   // Lightest tint
const TColor PRIMARY_100 = (TColor)0xFEF2E0;
const TColor PRIMARY_500 = (TColor)4227136;    // Main green  #408040
const TColor PRIMARY_600 = (TColor)0x306030;   // Darker (hover state)
const TColor PRIMARY_700 = (TColor)0x204020;
const TColor PRIMARY_900 = (TColor)0x102010;   // Darkest

// ---------------------------------------------------------------------------
// ACCENT — Violet highlights (badges, special indicators)
// ---------------------------------------------------------------------------
const TColor ACCENT_500 = (TColor)0xF65C8B;    // #8B5CF6
const TColor ACCENT_600 = (TColor)0xED3A7C;    // #7C3AED

// ---------------------------------------------------------------------------
// SEMANTIC COLORS — Success, Warning, Danger
// Used for status badges, trend indicators, validation feedback
// ---------------------------------------------------------------------------

// Success — Emerald Green (paid invoices, positive trends)
const TColor SUCCESS_50  = (TColor)0xF4FDF0;
const TColor SUCCESS_500 = (TColor)0x5EC522;   // #22C55E
const TColor SUCCESS_600 = (TColor)0x4AA316;

// Warning — Amber (overdue invoices, low stock alerts)
const TColor WARNING_50  = (TColor)0xEBFBFF;
const TColor WARNING_500 = (TColor)0x0B9EF5;   // #F59E0B
const TColor WARNING_600 = (TColor)0x0677D9;

// Danger — Red (negative trends, delete actions, errors)
const TColor DANGER_50  = (TColor)0xF2F2FE;
const TColor DANGER_500 = (TColor)0x4444EF;    // #EF4444
const TColor DANGER_600 = (TColor)0x2626DC;

// ---------------------------------------------------------------------------
// NEUTRAL GRAY SCALE — Used for text, borders, backgrounds
// Follows the 50-900 convention: 50 = lightest, 900 = darkest
// ---------------------------------------------------------------------------
const TColor GRAY_50  = (TColor)0xFAFAFA;
const TColor GRAY_100 = (TColor)0xF5F4F4;      // Card backgrounds
const TColor GRAY_200 = (TColor)0xE7E4E4;      // Borders
const TColor GRAY_300 = (TColor)0xD8D4D4;      // Disabled borders
const TColor GRAY_400 = (TColor)0xAAA1A1;      // Placeholder text
const TColor GRAY_500 = (TColor)0x7A7171;      // Secondary text, labels
const TColor GRAY_600 = (TColor)0x5B5252;      // Body text (light theme)
const TColor GRAY_700 = (TColor)0x463F3F;
const TColor GRAY_800 = (TColor)0x2A2727;      // Headings
const TColor GRAY_900 = (TColor)0x1B1818;      // Primary text, values

// ---------------------------------------------------------------------------
// BACKGROUNDS — Page-level surface colors
// ---------------------------------------------------------------------------
const TColor BG_PRIMARY   = (TColor)15790320;  // Main page bg    #F0F0F0
const TColor BG_SECONDARY = GRAY_50;           // Card surfaces
const TColor BG_TERTIARY  = GRAY_100;          // Nested panels

// ---------------------------------------------------------------------------
// SIDEBAR — Specific colors for the navigation sidebar
// ---------------------------------------------------------------------------
const TColor SIDEBAR_BG   = (TColor)4227136;   // Same as PRIMARY_500
const TColor SIDEBAR_TEXT = (TColor)clWhite;   // White text on green

// ---------------------------------------------------------------------------
// DARK MODE (prepared but not yet active — planned for v2.2)
// ---------------------------------------------------------------------------
const TColor DARK_BG_PRIMARY   = (TColor)0x2A170F;   // #0F172A  (darkest)
const TColor DARK_BG_SECONDARY = (TColor)0x3B291E;   // #1E293B
const TColor DARK_BG_TERTIARY  = (TColor)0x554133;   // #334155

} // namespace ModernColors

#endif
