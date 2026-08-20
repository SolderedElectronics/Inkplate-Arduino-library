/**
 **************************************************
 * @file        Inkplate13SPECTRA_Random_Art_Frame.ino
 * @brief       Generative art frame for Soldered Inkplate 13SPECTRA - a new
 *              random piece on every refresh.
 *
 * @details     Renders one piece of generative artwork, performs a single
 *              full-panel refresh and then deep-sleeps until the next redraw.
 *              No WiFi, no server and no SD card are needed - the whole picture
 *              is synthesised on the ESP32 from a random seed, so every refresh
 *              produces a different design.
 *
 *              Four generators are picked at random:
 *              0  FLOW FIELD  particles advected through layered value noise
 *              1  TRUCHET     quarter-arc tiles, randomly oriented
 *              2  PACKING     greedy non-overlapping circle packing
 *              3  PARTITION   recursive rectangular subdivision
 *
 *              The palette is randomized too: two to five inks are drawn from
 *              black, yellow, red, blue and green, shuffled so the first one
 *              dominates and the rest read as accents. White is never picked as
 *              an ink because it is the paper the artwork sits on. The caption
 *              strip prints a random edition number, the generator, how many
 *              inks were used and the swatches of those inks.
 *
 *              The six-ink Spectra palette is used the way a risograph printer
 *              uses its drums: flat, unmixed colour, no dithering and no
 *              grayscale. The library's INKPLATE_* colour macros are panel
 *              nibble values rather than framebuffer indices, which is why the
 *              stock examples write "INKPLATE_BLUE - 1"; this sketch defines its
 *              own C_* constants holding the indices drawPixel() expects
 *              (anything above 5 is silently dropped).
 *
 *              After drawing, the ESP32 sleeps for REFRESH_MINUTES and the
 *              sketch restarts from setup() on wake, drawing a new piece. The
 *              seed mixes the hardware RNG with a boot counter kept in ESP32
 *              deep-sleep memory, so every wake renders a different piece.
 *
 *              Expected output: a full-screen artwork with a caption strip
 *              along the bottom edge, redrawn every REFRESH_MINUTES, plus the
 *              chosen generator, palette and render time on Serial.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable (battery optional)
 * - Extra:      None
 * - Serial:     115200 baud (optional, for render logs)
 * - Settings (edit at the top of the sketch, or override with -D):
 *   - REFRESH_MINUTES  minutes of deep sleep between redraws
 *   - FORCE_GENERATOR  -1 picks at random, or 0..3 to lock one generator
 *   - MARGIN           white border around the artwork, in pixels
 *   - CAPTION          1 = print the caption strip and keyline
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Optionally change REFRESH_MINUTES, FORCE_GENERATOR, MARGIN or CAPTION.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) A random artwork is rendered and shown with a single full refresh.
 * 5) The board deep-sleeps for REFRESH_MINUTES, then wakes and draws a new,
 *    different piece.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-08-18
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h" // Include the Inkplate library
#include <esp_system.h>
#include <math.h>

// Configuration

#ifndef REFRESH_MINUTES
#define REFRESH_MINUTES  60     // minutes of deep sleep between redraws
#endif
#ifndef FORCE_GENERATOR
#define FORCE_GENERATOR  -1     // -1 = pick at random, or 0..3 to lock one generator
#endif
#ifndef MARGIN
#define MARGIN           64     // white border around the artwork, in px
#endif
#ifndef CAPTION
#define CAPTION          1      // 1 = print the museum-style caption strip
#endif

#define uS_TO_S_FACTOR   1000000ULL // Convert microseconds to seconds

// Framebuffer colour indices (see the note in @details above)
#define C_BLACK  0
#define C_WHITE  1
#define C_YELLOW 2
#define C_RED    3
#define C_BLUE   4
#define C_GREEN  5

Inkplate display; // Create an Inkplate object for Inkplate 13SPECTRA

// Artwork frame (set in setup once the panel geometry is known)
static int ART_X, ART_Y, ART_W, ART_H;

// Survives deep sleep, so every wake starts from a different seed
RTC_DATA_ATTR uint32_t bootCount = 0;

// Random number generator and value noise

static uint32_t rngState = 1;

static inline uint32_t rnd32()
{
    rngState ^= rngState << 13;
    rngState ^= rngState >> 17;
    rngState ^= rngState << 5;
    return rngState;
}
static inline float rndF()                     { return (rnd32() >> 8) * (1.0f / 16777216.0f); }
static inline float rndRange(float a, float b)  { return a + (b - a) * rndF(); }
static inline int   rndI(int n)                 { return (int)(rndF() * n) % (n > 0 ? n : 1); }

static inline uint32_t hash32(uint32_t x)
{
    x ^= x >> 16; x *= 0x7feb352dU;
    x ^= x >> 15; x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

// Deterministic lattice value in -1..1
static inline float lattice(int xi, int yi, uint32_t seed)
{
    uint32_t h = hash32((uint32_t)xi * 374761393U + (uint32_t)yi * 668265263U + seed);
    return (float)(h >> 8) * (1.0f / 8388608.0f) - 1.0f;
}

// Smooth 2D value noise
static float vnoise(float x, float y, uint32_t seed)
{
    int xi = (int)floorf(x), yi = (int)floorf(y);
    float xf = x - xi, yf = y - yi;
    float u = xf * xf * (3.0f - 2.0f * xf);
    float v = yf * yf * (3.0f - 2.0f * yf);

    float a = lattice(xi,     yi,     seed);
    float b = lattice(xi + 1, yi,     seed);
    float c = lattice(xi,     yi + 1, seed);
    float d = lattice(xi + 1, yi + 1, seed);

    float top = a + (b - a) * u;
    float bot = c + (d - c) * u;
    return top + (bot - top) * v;
}

// Fractal Brownian motion
static float fbm(float x, float y, uint32_t seed, int octaves)
{
    float sum = 0.0f, amp = 0.5f, freq = 1.0f;
    for (int i = 0; i < octaves; i++)
    {
        sum  += amp * vnoise(x * freq, y * freq, seed + (uint32_t)i * 7919U);
        freq *= 2.0f;
        amp  *= 0.5f;
    }
    return sum;
}

// Palettes

struct Palette
{
    char    name[8];   // caption label, e.g. "3 INKS"
    uint8_t n;         // number of inks in use, 2..5
    uint8_t c[5];      // the inks themselves, most dominant first
};

// The five Spectra inks that show up on the sheet. White is left out on purpose:
// it is the paper the artwork is drawn on, so a white "ink" would draw nothing.
static const uint8_t INKS[5] = {C_BLACK, C_YELLOW, C_RED, C_BLUE, C_GREEN};

static Palette pal;

// Build a random palette: shuffle the five inks, keep the first 2..5 of them.
// Because palPick() favours earlier entries, the first ink reads as the dominant
// colour and the rest as accents.
static void makePalette()
{
    uint8_t pool[5];
    for (int i = 0; i < 5; i++) pool[i] = INKS[i];

    for (int i = 4; i > 0; i--)     // Fisher-Yates shuffle
    {
        int     j = rndI(i + 1);
        uint8_t t = pool[i];
        pool[i]   = pool[j];
        pool[j]   = t;
    }

    pal.n = 2 + rndI(4);            // at least two inks, at most all five
    for (int i = 0; i < pal.n; i++) pal.c[i] = pool[i];
    for (int i = pal.n; i < 5; i++) pal.c[i] = C_WHITE;   // unused slots

    snprintf(pal.name, sizeof(pal.name), "%d INKS", pal.n);
}

// Biased pick: earlier entries dominate, later ones read as accents.
static inline uint8_t palPick()
{
    int a = rndI(pal.n), b = rndI(pal.n);
    return pal.c[a < b ? a : b];
}
static inline uint8_t palAny() { return pal.c[rndI(pal.n)]; }

// Clipped drawing helpers

static inline bool inArt(int x, int y)
{
    return x >= ART_X && y >= ART_Y && x < ART_X + ART_W && y < ART_Y + ART_H;
}

// Clipped filled disc - the workhorse for every stroked generator.
static void artDot(int cx, int cy, int r, uint8_t color)
{
    if (r <= 0)
    {
        if (inArt(cx, cy)) display.drawPixel(cx, cy, color);
        return;
    }
    int r2 = r * r;
    for (int dy = -r; dy <= r; dy++)
    {
        int y = cy + dy;
        if (y < ART_Y || y >= ART_Y + ART_H) continue;
        for (int dx = -r; dx <= r; dx++)
        {
            if (dx * dx + dy * dy > r2) continue;
            int x = cx + dx;
            if (x < ART_X || x >= ART_X + ART_W) continue;
            display.drawPixel(x, y, color);
        }
    }
}

// Clipped thick line (used by the partition generator)
static void artLine(int x0, int y0, int x1, int y1, int r, uint8_t color)
{
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int steps = (dx > dy ? dx : dy);
    if (steps == 0) { artDot(x0, y0, r, color); return; }
    for (int i = 0; i <= steps; i++)
    {
        artDot(x0 + (x1 - x0) * i / steps, y0 + (y1 - y0) * i / steps, r, color);
    }
}

// Quarter arc, centre (cx,cy), sweeping from a0 to a0+PI/2
static void artArc(float cx, float cy, float radius, float a0, int r, uint8_t color)
{
    int steps = (int)(radius * 2.2f) + 6;
    for (int i = 0; i <= steps; i++)
    {
        float a = a0 + (float)M_PI_2 * (float)i / (float)steps;
        artDot((int)lrintf(cx + cosf(a) * radius),
               (int)lrintf(cy + sinf(a) * radius), r, color);
    }
}

// Generator 0: flow field

static void genFlowField()
{
    uint32_t nseed   = rnd32();
    float    scale   = rndRange(0.0016f, 0.0042f);   // noise frequency in px^-1
    int      octaves = 2 + rndI(2);
    float    turns   = rndRange(1.4f, 3.4f);         // how many full rotations the field spans
    int      nPart   = 1400 + rndI(1600);
    int      maxStep = 90 + rndI(120);
    float    stepLen = rndRange(2.0f, 3.4f);
    bool     tapered = rndF() < 0.6f;

    for (int p = 0; p < nPart; p++)
    {
        float x = ART_X + rndF() * ART_W;
        float y = ART_Y + rndF() * ART_H;

        uint8_t color = palPick();
        int     baseR = 1 + rndI(3);
        int     life  = 20 + rndI(maxStep);

        for (int s = 0; s < life; s++)
        {
            float n = fbm(x * scale, y * scale, nseed, octaves);
            float a = n * (float)(2.0 * M_PI) * turns;

            x += cosf(a) * stepLen;
            y += sinf(a) * stepLen;
            if (!inArt((int)x, (int)y)) break;

            int r = baseR;
            if (tapered)
            {
                // fade the stroke out towards its tail
                float t = 1.0f - (float)s / (float)life;
                r = (int)lrintf(baseR * (0.35f + 0.65f * t));
            }
            artDot((int)x, (int)y, r, color);
        }
    }
}

// Generator 1: truchet

static void genTruchet()
{
    const int sizes[] = {50, 60, 75, 100, 120};
    int  S      = sizes[rndI(5)];
    int  stroke = 2 + rndI(4);
    bool tinted = rndF() < 0.45f;      // occasionally fill tile backgrounds

    int cols = ART_W / S;
    int rows = ART_H / S;
    int offX = ART_X + (ART_W - cols * S) / 2;
    int offY = ART_Y + (ART_H - rows * S) / 2;

    // Pass 1: tile backgrounds. Done first so a tinted tile can never clip the arc
    // that spills over from the tile next to it.
    if (tinted)
    {
        uint32_t bgSeed = rnd32();
        for (int gy = 0; gy < rows; gy++)
            for (int gx = 0; gx < cols; gx++)
            {
                uint32_t h = hash32(bgSeed + (uint32_t)gy * 92837111U + (uint32_t)gx * 689287499U);
                if ((h & 0xFFFF) > (uint32_t)(0.18f * 65535.0f)) continue;

                uint8_t bg = pal.c[(h >> 16) % pal.n];
                int     tx = offX + gx * S, ty = offY + gy * S;
                for (int y = ty; y < ty + S; y++)
                    for (int x = tx; x < tx + S; x++)
                        if (inArt(x, y)) display.drawPixel(x, y, bg);
            }
    }

    // Pass 2: the arcs themselves
    for (int gy = 0; gy < rows; gy++)
    {
        for (int gx = 0; gx < cols; gx++)
        {
            int     tx    = offX + gx * S;
            int     ty    = offY + gy * S;
            uint8_t color = palPick();
            float   half  = S * 0.5f;

            if (rnd32() & 1)
            {
                artArc(tx,     ty,     half, 0.0f,           stroke, color);  // TL corner
                artArc(tx + S, ty + S, half, (float)M_PI,    stroke, color);  // BR corner
            }
            else
            {
                artArc(tx + S, ty,     half, (float)M_PI_2,  stroke, color);  // TR corner
                artArc(tx,     ty + S, half, -(float)M_PI_2, stroke, color);  // BL corner
            }
        }
    }
}

// Generator 2: packing

#define MAX_CIRCLES 1100

struct Circ { float x, y, r; };
static Circ circles[MAX_CIRCLES];

static void genPacking()
{
    int   count    = 0;
    int   attempts = 26000;
    float gap      = rndRange(3.0f, 9.0f);
    float minR     = rndRange(4.0f, 9.0f);
    float maxR     = rndRange(60.0f, 150.0f);
    bool  outline  = rndF() < 0.7f;
    bool  rings    = rndF() < 0.35f;

    for (int a = 0; a < attempts && count < MAX_CIRCLES; a++)
    {
        float x = ART_X + rndF() * ART_W;
        float y = ART_Y + rndF() * ART_H;

        // largest radius that still fits inside the frame
        float r = maxR;
        float dl = x - ART_X, dr = (ART_X + ART_W) - x;
        float dt = y - ART_Y, db = (ART_Y + ART_H) - y;
        if (dl < r) r = dl;
        if (dr < r) r = dr;
        if (dt < r) r = dt;
        if (db < r) r = db;

        for (int i = 0; i < count && r >= minR; i++)
        {
            float ddx = x - circles[i].x, ddy = y - circles[i].y;
            float d   = sqrtf(ddx * ddx + ddy * ddy) - circles[i].r - gap;
            if (d < r) r = d;
        }
        if (r < minR) continue;

        circles[count].x = x;
        circles[count].y = y;
        circles[count].r = r;
        count++;

        uint8_t color = palPick();
        int     ri    = (int)lrintf(r);

        if (rings && ri > 26)
        {
            // concentric rings instead of a flat disc
            uint8_t alt = palAny();
            for (int rr = ri; rr > 2; rr -= 6 + rndI(6))
                artDot((int)x, (int)y, rr, (rr / 6) & 1 ? color : alt);
        }
        else
        {
            artDot((int)x, (int)y, ri, color);
        }

        if (outline && color != C_BLACK && ri > 6)
        {
            // thin keyline: draw the ring by stamping along the circumference
            int steps = ri * 5 + 8;
            for (int s = 0; s < steps; s++)
            {
                float ang = (float)(2.0 * M_PI) * s / steps;
                artDot((int)lrintf(x + cosf(ang) * ri),
                       (int)lrintf(y + sinf(ang) * ri), 1, C_BLACK);
            }
        }
    }
}

// Generator 3: partition

static int   partGutter;
static int   partMinCell;
static float partFillChance;

static void partition(int x, int y, int w, int h, int depth)
{
    bool leaf = (depth >= 7) || (w < partMinCell * 2 && h < partMinCell * 2) ||
                (depth >= 3 && rndF() < 0.20f);

    if (leaf)
    {
        // Big cells get inked far less often than small ones - otherwise one huge
        // slab of red swallows the whole composition.
        float area  = (float)w * (float)h / ((float)ART_W * (float)ART_H);
        float bias  = 1.0f / (1.0f + 14.0f * area);
        uint8_t color = (rndF() < partFillChance * bias) ? palPick() : C_WHITE;

        for (int yy = y; yy < y + h; yy++)
            for (int xx = x; xx < x + w; xx++)
                if (inArt(xx, yy)) display.drawPixel(xx, yy, color);

        // keyline around every cell
        artLine(x,         y,         x + w - 1, y,         1, C_BLACK);
        artLine(x,         y + h - 1, x + w - 1, y + h - 1, 1, C_BLACK);
        artLine(x,         y,         x,         y + h - 1, 1, C_BLACK);
        artLine(x + w - 1, y,         x + w - 1, y + h - 1, 1, C_BLACK);

        // occasional inhabitant so the grid does not read as pure Mondrian
        if (color == C_WHITE && w > 60 && h > 60 && rndF() < 0.35f)
        {
            uint8_t acc = palPick();
            int     pad = 12;
            switch (rndI(3))
            {
            case 0:
                artDot(x + w / 2, y + h / 2, (w < h ? w : h) / 2 - pad, acc);
                break;
            case 1:
                artLine(x + pad, y + pad, x + w - pad, y + h - pad, 3, acc);
                break;
            default:
                artArc(x + pad, y + pad, (float)((w < h ? w : h) - 2 * pad), 0.0f, 3, acc);
                break;
            }
        }
        return;
    }

    bool splitVertical = (w > h);
    if (fabsf((float)(w - h)) < 0.25f * (float)w) splitVertical = (rnd32() & 1);

    float t = rndRange(0.32f, 0.68f);
    if (splitVertical)
    {
        int cut = (int)(w * t);
        partition(x,                        y, cut - partGutter / 2,     h, depth + 1);
        partition(x + cut + partGutter / 2, y, w - cut - partGutter / 2, h, depth + 1);
    }
    else
    {
        int cut = (int)(h * t);
        partition(x, y,                        w, cut - partGutter / 2,     depth + 1);
        partition(x, y + cut + partGutter / 2, w, h - cut - partGutter / 2, depth + 1);
    }
}

static void genPartition()
{
    partGutter     = 4 + rndI(14);
    partMinCell    = 48 + rndI(90);
    partFillChance = rndRange(0.35f, 0.75f);
    partition(ART_X, ART_Y, ART_W, ART_H, 0);
}

// Caption strip

static const char *GEN_NAMES[] = {"FLOW FIELD", "TRUCHET", "PACKING", "PARTITION"};

static void drawFurniture(int genIndex, uint32_t edition)
{
#if CAPTION
    int W = display.width(), H = display.height();

    // hairline keyline around the plate
    display.drawRect(MARGIN - 10, MARGIN - 10, W - 2 * (MARGIN - 10), H - 2 * (MARGIN - 10), C_BLACK);

    display.setTextSize(2);
    display.setTextColor(C_BLACK, C_WHITE);

    char line[96];
    snprintf(line, sizeof(line), "No. %04lu   %s   %s",
             (unsigned long)(edition % 10000UL), GEN_NAMES[genIndex], pal.name);

    display.setCursor(MARGIN - 10, H - MARGIN + 12);
    display.print(line);

    // palette swatches, right aligned
    int sw = 26, sx = W - MARGIN + 10 - pal.n * (sw + 6);
    for (int i = 0; i < pal.n; i++)
    {
        display.fillRect(sx + i * (sw + 6), H - MARGIN + 8, sw, sw, pal.c[i]);
        display.drawRect(sx + i * (sw + 6), H - MARGIN + 8, sw, sw, C_BLACK);
    }
#endif
}

// Main

void setup()
{
    Serial.begin(115200);      // Init serial communication
    display.begin();           // Initialize the display hardware

    ART_X = MARGIN;
    ART_Y = MARGIN;
    ART_W = display.width()  - 2 * MARGIN;
    ART_H = display.height() - 2 * MARGIN - (CAPTION ? 48 : 0);

    // Fresh seed on every refresh: hardware RNG mixed with the deep sleep boot
    // counter, so each wake renders a different piece.
    bootCount++;
    rngState = hash32(esp_random() ^ hash32(bootCount * 2654435761UL));
    if (rngState == 0) rngState = 1;

    uint32_t edition = rnd32();
    makePalette();

    int gen = (FORCE_GENERATOR >= 0) ? FORCE_GENERATOR : rndI(4);

    Serial.printf("Edition %04lu | generator %s | palette %s\n",
                  (unsigned long)(edition % 10000UL), GEN_NAMES[gen], pal.name);

    display.clearDisplay();      // Clear the frame buffer (does NOT clear the physical screen)
    display.fillScreen(C_WHITE); // Start from a white sheet

    uint32_t t0 = millis();
    switch (gen)
    {
    case 0:  genFlowField(); break;
    case 1:  genTruchet();   break;
    case 2:  genPacking();   break;
    default: genPartition(); break;
    }
    Serial.printf("Rendered in %lu ms\n", (unsigned long)(millis() - t0));

    drawFurniture(gen, edition);

    display.display();  // Refresh the e-paper display (full refresh, ~30 s on Spectra 6)

    // Sleep until the next redraw
    Serial.printf("Going to sleep for %d minutes\n", REFRESH_MINUTES);
    esp_sleep_enable_timer_wakeup((uint64_t)REFRESH_MINUTES * 60ULL * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you use deep sleep, the whole program should be in setup() because the board
    // restarts each time. loop() must be empty!
}
