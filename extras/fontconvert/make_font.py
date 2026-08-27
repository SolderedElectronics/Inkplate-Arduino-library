#!/usr/bin/env python3
"""
make_font.py - build an Adafruit_GFX / GFXfont .h header straight from a TTF,
including any extra Unicode characters you need (accents, umlauts, etc), in
ONE step. No C compiler, no FreeType dev headers/libs to hunt down per
platform - the only dependency is the "freetype-py" package, which ships
prebuilt FreeType binaries for Windows/macOS/Linux, so:

    pip install freetype-py

is the entire setup. This replaces the old two-step flow (build
extras/fontconvert/fontconvert with a C compiler + system FreeType headers,
then splice its output onto an existing base header) with a single Python
script that does both jobs at once: render 7-bit ASCII (0x20-0x7E) AND any
extra characters you ask for, from the SAME .ttf, in the SAME run. The C
tool (extras/fontconvert/fontconvert.c) is still there and still works if
you'd rather use it, but this script is the simpler path for adding custom
glyphs.

WHY YOU NEED THIS AT ALL (background)
--------------------------------------
Inkplate's text drawing (Adafruit_GFX::write()/drawChar() under the hood)
looks up glyphs with a single BYTE index: glyphTable[code - font->first].
There is no Unicode decoding anywhere in that path, so:
  - A character above 0xFF can never be printed via print()/write(), no
    matter what a .ttf file contains.
  - Typing an accented character directly into a string literal doesn't work
    either: Arduino source is UTF-8, so e.g. 'ä' is stored as two bytes
    (0xC3 0xA4), and print() would draw two WRONG glyphs instead of one.

The fix: give every extra character its own spot at an unused BYTE code
(this script assigns them right after 0x7E, e.g. 0x7F, 0x80, 0x81, ... with
zero gaps - GFXfont has no sparse/lookup table, so even a single skipped
code silently shifts every glyph after it and corrupts the last one), then
print those bytes explicitly in your sketch instead of the literal
character - see the CH_* macros in Inkplate13SPECTRA_Custom_Diacritics.ino
for the pattern.

USAGE
-----
    pip install freetype-py

    python3 make_font.py /path/to/Font.ttf ä ö ü ß Ä Ö Ü

    # Optional overrides:
    python3 make_font.py /path/to/Font.ttf č ć đ š ž --size 24 \\
        --out MyFont24pt7b.h --name MyFont24pt7b

    ttf      (positional, required) path to a .ttf/.otf. Must actually
             contain every requested glyph - the script errors out (doesn't
             silently skip) if one is missing.
    chars    (positional, optional) the extra characters to add on top of
             7-bit ASCII (0x20-0x7E), space-separated, e.g. "ä ö ü ß Ä Ö Ü".
             A single argument can also be a whole run of characters typed
             together (e.g. "äöüß") - it gets split automatically. Omit
             entirely to just rebuild plain ASCII from a different .ttf.
    --size   point size (default 18). DPI is fixed at 141, matching
             Adafruit's original fontconvert tool, so a given size looks
             the same as the bundled Fonts/*.h.
    --out    output header path (default: <name>.h in the current directory)
    --name   C identifier prefix for the font (default: derived from the
             ttf filename + size + bit-depth, same convention the original
             fontconvert tool used, e.g. "FreeSans18pt8b")

The script prints the byte code assigned to each extra character when done -
paste that straight into #define macros in your sketch, same as before.
"""
import argparse
import ctypes
import os
import re
import sys

try:
    import freetype
except ImportError:
    sys.exit(
        "Missing dependency 'freetype-py'.\n"
        "Install it with:  pip install freetype-py\n"
        "(no C compiler or system FreeType needed - it's a prebuilt wheel)"
    )

DPI = 141  # matches extras/fontconvert/fontconvert.c
TT_INTERPRETER_VERSION_35 = 35  # matches extras/fontconvert/fontconvert.c: classic (non-subpixel)
                                 # TrueType hinting, so glyph bitmaps match the bundled Fonts/*.h
                                 # pixel-for-pixel instead of drifting with whatever hinting a given
                                 # FreeType build defaults to.


def use_classic_tt_hinting():
    version = ctypes.c_uint(TT_INTERPRETER_VERSION_35)
    freetype.FT_Property_Set(
        freetype.get_handle(), b"truetype", b"interpreter-version", ctypes.byref(version)
    )


def derive_name(ttf_path, size, last_code):
    base = os.path.basename(ttf_path)
    base = os.path.splitext(base)[0]
    base = re.sub(r"[^0-9A-Za-z_]", "_", base)
    bits = 8 if last_code > 127 else 7
    return f"{base}{size}pt{bits}b"


def render_glyph(face, codepoint):
    face.load_char(codepoint, freetype.FT_LOAD_TARGET_MONO)
    face.glyph.render(freetype.FT_RENDER_MODE_MONO)
    slot = face.glyph
    bitmap = slot.bitmap

    bits = []
    for y in range(bitmap.rows):
        row_start = y * bitmap.pitch
        for x in range(bitmap.width):
            byte = bitmap.buffer[row_start + x // 8]
            mask = 0x80 >> (x & 7)
            bits.append(1 if (byte & mask) else 0)

    width, height = bitmap.width, bitmap.rows
    if not any(bits):
        # Some FreeType builds return a spurious 1x1 all-zero bitmap for
        # ink-free glyphs (e.g. space) instead of a proper 0x0 one. Collapse
        # to 0x0 either way - nothing is drawn, and it keeps a blank glyph
        # from wasting a bitmap byte.
        width, height = 0, 0
        bits = []

    # pack MSB-first, padded to a byte boundary at the END of the glyph
    # (not per scanline) - matches extras/fontconvert/fontconvert.c's enbit()
    out = bytearray()
    acc = 0
    nbits = 0
    for b in bits:
        acc = (acc << 1) | b
        nbits += 1
        if nbits == 8:
            out.append(acc)
            acc = 0
            nbits = 0
    if nbits:
        acc <<= (8 - nbits)
        out.append(acc)

    x_advance = slot.advance.x >> 6
    x_offset = slot.bitmap_left if width else 0
    y_offset = (1 - slot.bitmap_top) if height else 1
    return list(out), width, height, x_advance, x_offset, y_offset


def hex_bytes(byte_list):
    return ["0x%02X" % b for b in byte_list]


def format_bitmap_block(all_bytes, per_line=12):
    lines = []
    for i in range(0, len(all_bytes), per_line):
        lines.append(", ".join(all_bytes[i:i + per_line]))
    return ",\n  ".join(lines)


def main():
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='Example: python3 make_font.py FreeSans.ttf š ć č Č "đ" ž',
    )
    ap.add_argument("ttf", help="source .ttf/.otf")
    ap.add_argument(
        "chars", nargs="*",
        help='extra characters to add on top of 7-bit ASCII, e.g.: š ć č Č đ ž '
             '(each argument can also be a run of several characters, e.g. "šćčĐž")',
    )
    ap.add_argument("--size", type=int, default=18, help="point size (default 18)")
    ap.add_argument("--out", default=None, help="output .h path (default: <name>.h)")
    ap.add_argument("--name", default=None, help="C identifier prefix (default: derived from ttf/size)")
    args = ap.parse_args()

    # each positional arg may itself be several characters typed together
    # (shell quoting a whole word) - split those out, then de-duplicate
    # while keeping first-seen order.
    extra_chars = []
    for arg in args.chars:
        for ch in arg:
            if ch not in extra_chars:
                extra_chars.append(ch)

    ascii_first, ascii_last = 0x20, 0x7E
    n_ascii = ascii_last - ascii_first + 1

    if n_ascii + len(extra_chars) > 256:
        sys.exit(
            f"ASCII (0x20-0x7E) + {len(extra_chars)} extra glyphs doesn't fit "
            f"in a byte range (GFXfont first/last are uint8_t)"
        )
    last_code = ascii_last + len(extra_chars)

    name = args.name or derive_name(args.ttf, args.size, last_code)
    out_path = args.out or f"{name}.h"

    use_classic_tt_hinting()
    try:
        face = freetype.Face(args.ttf)
    except Exception as e:
        sys.exit(f"could not open '{args.ttf}': {e}")
    face.set_char_size(args.size * 64, 0, DPI, 0)

    all_bitmaps = []
    glyph_lines = []
    legend = []  # (code, char) for extras only

    def add_glyph(code, codepoint, comment):
        try:
            gbytes, w, h, xadv, xoff, yoff = render_glyph(face, codepoint)
        except Exception as e:
            sys.exit(f"failed to render U+{codepoint:04X} ('{chr(codepoint)}'): {e}")
        offset = len(all_bitmaps)
        all_bitmaps.extend(hex_bytes(gbytes))
        return offset, w, h, xadv, xoff, yoff, comment

    entries = []
    for code in range(ascii_first, ascii_last + 1):
        comment = "'%s'" % chr(code)
        entries.append((code,) + add_glyph(code, code, comment))

    for i, ch in enumerate(extra_chars):
        code = ascii_last + 1 + i
        cp = ord(ch)
        comment = "'%s' (U+%04X)" % (ch, cp)
        entries.append((code,) + add_glyph(code, cp, comment))
        legend.append((code, ch, cp))

    for i, (code, offset, w, h, xadv, xoff, yoff, comment) in enumerate(entries):
        is_last = i == len(entries) - 1
        core = "  { %5d, %3d, %3d, %3d, %4d, %4d }" % (offset, w, h, xadv, xoff, yoff)
        if is_last:
            glyph_lines.append(f"{core} }}; // {code:#04x} {comment}")
        else:
            glyph_lines.append(f"{core},   // {code:#04x} {comment}")

    if face.size.height == 0:
        y_advance = entries[0][3]  # fall back to first glyph's height, like fontconvert.c
    else:
        y_advance = face.size.height >> 6

    bitmaps_block = format_bitmap_block(all_bitmaps)

    out_text = (
        f"const uint8_t {name}Bitmaps[] PROGMEM = {{\n  {bitmaps_block} }};\n\n"
        f"const GFXglyph {name}Glyphs[] PROGMEM = {{\n"
        + "\n".join(glyph_lines) + "\n\n"
        f"const GFXfont {name} PROGMEM = {{\n"
        f"  (uint8_t  *){name}Bitmaps,\n"
        f"  (GFXglyph *){name}Glyphs,\n"
        f"  {ascii_first:#04x}, {last_code:#04x}, {y_advance} }};\n"
    )

    with open(out_path, "w", encoding="utf-8") as f:
        f.write(out_text)

    print(f"wrote {out_path}")
    print(f"font '{name}' covers {ascii_first:#04x}..{last_code:#04x} ({len(entries)} glyphs)")
    if legend:
        print("extra glyph legend:")
        for code, ch, cp in legend:
            safe = ch if re.match(r"[0-9A-Za-z]", ch) else ("U%04X" % cp)
            print(f'  #define CH_{safe} "\\x{code:02x}"  // {ch}  U+{cp:04X}')


if __name__ == "__main__":
    main()
