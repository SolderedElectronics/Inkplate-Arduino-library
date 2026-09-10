/**
 **************************************************
 * @file        Inkplate13SPECTRA_Custom_Diacritics.ino
 * @brief       HOW-TO: add custom Unicode glyphs (German ä ö ü ß Ä Ö Ü here)
 *              to a GFX font, for Soldered Inkplate 13SPECTRA. See
 *              extras/fontconvert/make_font.py to do this for any language.
 *
 * @details     THE PROBLEM
 *              ------------
 *              The bundled GFX fonts (FreeSans/FreeSerif/FreeMono, see
 *              Fonts/) only cover 7-bit ASCII (0x20-0x7E). Adafruit_GFX's
 *              text renderer (Adafruit_GFX::write()/drawChar(), which
 *              Inkplate's print()/write() calls into) looks up glyphs with a
 *              single BYTE index: glyphTable[code - font->first]. There is
 *              no Unicode decoding anywhere in that path.
 *
 *              So a character above 0xFF can NEVER be printed via
 *              print()/write(), no matter what a .ttf file contains -
 *              rebuilding a font from a TTF that "has" ä/ö/ü doesn't help by
 *              itself. Worse, typing ä/ö/ü/ß directly into a string literal
 *              in the .ino doesn't work either: the Arduino source file is
 *              UTF-8, so e.g. 'ä' (U+00E4) is stored as the TWO bytes 0xC3
 *              0xA4, and print() would look up (and draw) two wrong glyphs
 *              at codes 0xC3 and 0xA4 instead of the one you wanted.
 *
 *              THE FIX - ONE COMMAND, ANY LANGUAGE
 *              --------------------------------------
 *              extras/fontconvert/make_font.py builds a complete GFXfont
 *              header (7-bit ASCII + whatever extra characters you name)
 *              straight from a .ttf, in one step. Its only dependency is the
 *              "freetype-py" package - no C compiler, no hunting for
 *              FreeType dev headers/libs per platform. Easiest way to run it
 *              (make_font.sh/.bat set up a local, throwaway virtualenv next
 *              to themselves and install freetype-py into THAT, not your
 *              system Python - safe to re-run, nothing global gets touched):
 *
 *                macOS/Linux:  extras/fontconvert/make_font.sh  FreeSans.ttf ä ö ü ß Ä Ö Ü --size 18 --out FreeSansGerman18pt7b.h --name FreeSansGerman18pt7b
 *                Windows:      extras\fontconvert\make_font.bat FreeSans.ttf ä ö ü ß Ä Ö Ü --size 18 --out FreeSansGerman18pt7b.h --name FreeSansGerman18pt7b
 *
 *              (or set up the venv yourself and call make_font.py directly -
 *              see its docstring for the manual steps and full option list)
 *
 *              That's the exact command used to generate
 *              FreeSansGerman18pt7b.h in this folder. The script prints the
 *              byte code it assigned each character - that's where the CH_*
 *              #defines below came from. For a different language, swap the
 *              character list (and the .ttf path if you want a different
 *              source font); nothing else changes. See make_font.py's own
 *              docstring for the full algorithm explanation, including the
 *              two rules that matter if you ever want to understand or
 *              hand-edit the output:
 *
 *              1. Every extra character must actually exist in the source
 *                 .ttf - the script errors out loudly (not silently) if one
 *                 is missing.
 *              2. GFXfont requires an entry for EVERY byte code between
 *                 first and last, with nothing skipped - there's no sparse
 *                 lookup, just direct array indexing. Leave a gap and every
 *                 glyph after it silently renders as its neighbour's shape,
 *                 with the very last one reading past the end of the array
 *                 (visual noise on screen). make_font.py sidesteps this by
 *                 always assigning new codes starting right after the last
 *                 ASCII one (0x7E), counting up with zero gaps - here that's
 *                 0x7F through 0x85.
 *
 *              In the sketch, print the extra characters via their assigned
 *              byte codes - either display.write((uint8_t)0x7F), or (used
 *              below, reads better inside a sentence) adjacent
 *              string-literal concatenation: CH_ae "chzt" expands to
 *              "\x7f" "chzt", i.e. one raw byte 0x7F followed by the literal
 *              text "chzt". Never type the accented character itself into
 *              the string - see "THE PROBLEM" above for why.
 *
 *              GNU FreeFont (the family Fonts/Free*.h were built from) has
 *              broad Unicode coverage, including Latin-1 Supplement (which
 *              is where ä/ö/ü/ß/Ä/Ö/Ü live) and Latin Extended-A (accented
 *              Central/Eastern European letters) - it's a good first place
 *              to look for a source .ttf: https://savannah.gnu.org/projects/freefont/
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) The German sentence and the full extra-glyph legend appear on screen.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-08-27
 **************************************************/

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"
#include "FreeSansGerman18pt7b.h" // FreeSans18pt7b + ä ö ü ß Ä Ö Ü at 0x7F-0x85

// Byte codes the extra glyphs were placed at inside FreeSansGerman18pt7b.h
// (printed by make_font.py when the font was generated - see header comment
// above). Used as adjacent string literals, e.g. CH_AE "rger" -> "\x83" "rger".
#define CH_ae "\x7f" // ä  U+00E4
#define CH_oe "\x80" // ö  U+00F6
#define CH_ue "\x81" // ü  U+00FC
#define CH_sz "\x82" // ß  U+00DF
#define CH_AE "\x83" // Ä  U+00C4
#define CH_OE "\x84" // Ö  U+00D6
#define CH_UE "\x85" // Ü  U+00DC

Inkplate display;

void setup()
{
    display.begin();
    display.clearDisplay();
    display.setTextColor(INKPLATE_BLACK);
    display.setFont(&FreeSansGerman18pt7b);

    // A German sentence exercising every extra glyph: Ö, ä, ü (x2), ö, ß, Ü, Ä.
    display.setCursor(10, 60);
    display.print(F(CH_OE "sterreichische M" CH_ae "dchen " CH_ue "ben ihre Gr" CH_oe CH_sz "e."));
    display.setCursor(10, 100);
    display.print(F(CH_UE "ber " CH_AE "rger spricht niemand gern."));

    // Full legend of every extra glyph, upper- then lower-case.
    display.setCursor(10, 150);
    display.print(F(CH_AE CH_OE CH_UE " " CH_ae CH_oe CH_ue CH_sz));

    display.display();
}

void loop()
{
}
