/**
 **************************************************
 * @file        Gif.cpp
 * @brief       Basic functionalities to play GIF images/animations on
 *              black and white Inkplate displays
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact assistance@soldered.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE13SPECTRA) &&           \
    !defined(ARDUINO_INKPLATE31SPECTRA)
#include "../../system/defines.h"
#include "Gif.h"
#include "Inkplate.h"

/**
 * @brief       begin initialises the Gif subsystem. Must be called once
 *              before any draw/play functions are used.
 *
 * @param       Inkplate *inkplateptr
 *              Pointer to the parent Inkplate instance.
 */
void Gif::begin(Inkplate *inkplateptr)
{
    _inkplate = inkplateptr;
    _gif.begin(GIF_PALETTE_RGB565_LE);
}

/**
 * @brief       stop requests that a currently running playGifFromBuffer/Sd()
 *              call returns as soon as the current frame finishes. Safe to
 *              call from an ISR-set flag checked in the main loop.
 */
void Gif::stop()
{
    _stopRequested = true;
}

/**
 * @brief       gifDrawCallback is called by AnimatedGIF once per decoded
 *              scanline. Converts the 8-bit palette pixels of that line to
 *              1-bit using a fixed luminance threshold and writes them
 *              straight into the Inkplate framebuffer - no full-frame or
 *              full-row buffering is needed.
 */
void Gif::gifDrawCallback(GIFDRAW *pDraw)
{
    GifDrawContext *ctx = (GifDrawContext *)pDraw->pUser;
    Inkplate *inkplate = ctx->self->_inkplate;

    const int screenY = ctx->drawY + pDraw->iY + pDraw->y;
    const uint8_t *pixels = pDraw->pPixels;
    const uint16_t *palette = pDraw->pPalette;

    for (int col = 0; col < pDraw->iWidth; col++)
    {
        const uint8_t idx = pixels[col];
        if (pDraw->ucHasTransparency && idx == pDraw->ucTransparent)
            continue; // leave whatever is already on screen showing through

        const uint16_t rgb565 = palette[idx];
        const uint8_t r = _RED(rgb565), g = _GREEN(rgb565), b = _BLUE(rgb565);

        uint32_t val = RGB3BIT(r, g, b);
        if (ctx->invert)
            val = 7 - val;
        if (inkplate->getDisplayMode() == INKPLATE_1BIT)
            val = (~val >> 2) & 1;

        inkplate->drawPixel(ctx->drawX + pDraw->iX + col, screenY, val);
    }
}

/**
 * @brief       drawGifFromBuffer draws only the first frame of the GIF held
 *              in buf, e.g. to use it as a static image.
 *
 * @return      true if the frame was drawn, false on open/decode error.
 */
bool Gif::drawGifFromBuffer(uint8_t *buf, int32_t len, int x, int y, bool invert)
{
    if (!_inkplate)
        return false;

    if (!_gif.open(buf, len, gifDrawCallback))
        return false;

    GifDrawContext ctx{this, x, y, invert};
    int frameDelayMs = 0;
    const int rc = _gif.playFrame(false, &frameDelayMs, &ctx);
    _gif.close();

    return rc != -1;
}

/**
 * @brief       drawGifFromSd loads a whole GIF file from the SD card into a
 *              PSRAM buffer and draws only its first frame.
 *
 * @return      true if the frame was drawn, false if the file could not be
 *              read or decoded.
 */
bool Gif::drawGifFromSd(const char *fileName, int x, int y, bool invert)
{
    SdFile dat;
    if (!dat.open(fileName, O_RDONLY))
        return false;

    const uint32_t total = dat.fileSize();
    uint8_t *buf = (uint8_t *)ps_malloc(total);
    if (!buf)
    {
        dat.close();
        return false;
    }

    uint32_t pnt = 0;
    while (pnt < total)
    {
        const uint32_t toRead = dat.available();
        if (toRead > 0)
        {
            const int read = dat.read(buf + pnt, toRead);
            if (read > 0)
                pnt += read;
        }
    }
    dat.close();

    const bool ret = drawGifFromBuffer(buf, total, x, y, invert);
    free(buf);
    return ret;
}

/**
 * @brief       drawGifFromWeb downloads a whole GIF file from a http(s) URL
 *              into a PSRAM buffer and draws only its first frame.
 *
 * @return      true if the frame was drawn, false if the file could not be
 *              downloaded or decoded.
 */
bool Gif::drawGifFromWeb(const char *url, int x, int y, bool invert)
{
    int32_t defaultLen = E_INK_WIDTH * E_INK_HEIGHT * 4;
    uint8_t *buf = NULL;

    if (strncmp(url, "http://", 7) == 0)
        buf = _inkplate->downloadFile(url, &defaultLen);
    else if (strncmp(url, "https://", 8) == 0)
        buf = _inkplate->downloadFileHTTPS(url, &defaultLen);

    if (!buf)
        return false;

    const bool ret = drawGifFromBuffer(buf, defaultLen, x, y, invert);
    free(buf);
    return ret;
}

/**
 * @brief       playGifFromBuffer plays every frame of the GIF held in buf,
 *              blocking until finished (or forever if loop is true).
 *              e-ink partial refresh time (hundreds of ms) will normally
 *              dominate over the GIF's own frame delay, so real playback
 *              speed is limited by the panel, not by the file.
 *
 * @param       fullRefreshEveryFrames
 *              Forwarded to setFullUpdateThreshold(): forces a full refresh
 *              every N partialUpdate() calls to clear partial-update
 *              ghosting. Pass 0 to never force a full refresh.
 *
 * @param       leaveOn
 *              Forwarded to partialUpdate(): if true (default), panel power
 *              is left on between frames instead of being switched off and
 *              back on for every single frame.
 *
 * @return      true if playback ran to completion/was stopped cleanly,
 *              false on open/decode error.
 */
bool Gif::playGifFromBuffer(uint8_t *buf, int32_t len, int x, int y, bool invert, bool loop,
                            uint16_t fullRefreshEveryFrames, bool leaveOn)
{
    if (!_inkplate)
        return false;

    if (!_gif.open(buf, len, gifDrawCallback))
        return false;

    GifDrawContext ctx{this, x, y, invert};
    _stopRequested = false;
    _inkplate->setFullUpdateThreshold(fullRefreshEveryFrames);

    int frameDelayMs = 0;
    int rc;
    do
    {
        rc = _gif.playFrame(false, &frameDelayMs, &ctx);
        if (rc == -1)
            break;

        _inkplate->partialUpdate(false, leaveOn);

        if (frameDelayMs > 0)
            delay(frameDelayMs);
    } while ((rc == 1 || loop) && !_stopRequested);

    _gif.close();
    return rc != -1;
}

/**
 * @brief       playGifFromSd loads a whole GIF file from the SD card into a
 *              PSRAM buffer and plays every frame. See playGifFromBuffer().
 *
 * @return      true if playback ran to completion/was stopped cleanly,
 *              false if the file could not be read or decoded.
 */
bool Gif::playGifFromSd(const char *fileName, int x, int y, bool invert, bool loop, uint16_t fullRefreshEveryFrames,
                        bool leaveOn)
{
    SdFile dat;
    if (!dat.open(fileName, O_RDONLY))
        return false;

    const uint32_t total = dat.fileSize();
    uint8_t *buf = (uint8_t *)ps_malloc(total);
    if (!buf)
    {
        dat.close();
        return false;
    }

    uint32_t pnt = 0;
    while (pnt < total)
    {
        const uint32_t toRead = dat.available();
        if (toRead > 0)
        {
            const int read = dat.read(buf + pnt, toRead);
            if (read > 0)
                pnt += read;
        }
    }
    dat.close();

    const bool ret = playGifFromBuffer(buf, total, x, y, invert, loop, fullRefreshEveryFrames, leaveOn);
    free(buf);
    return ret;
}

/**
 * @brief       playGifFromWeb downloads a whole GIF file from a http(s) URL
 *              into a PSRAM buffer and plays every frame. See
 *              playGifFromBuffer().
 *
 * @return      true if playback ran to completion/was stopped cleanly,
 *              false if the file could not be downloaded or decoded.
 */
bool Gif::playGifFromWeb(const char *url, int x, int y, bool invert, bool loop, uint16_t fullRefreshEveryFrames,
                         bool leaveOn)
{
    int32_t defaultLen = E_INK_WIDTH * E_INK_HEIGHT * 4;
    uint8_t *buf = NULL;

    if (strncmp(url, "http://", 7) == 0)
        buf = _inkplate->downloadFile(url, &defaultLen);
    else if (strncmp(url, "https://", 8) == 0)
        buf = _inkplate->downloadFileHTTPS(url, &defaultLen);

    if (!buf)
        return false;

    const bool ret = playGifFromBuffer(buf, defaultLen, x, y, invert, loop, fullRefreshEveryFrames, leaveOn);
    free(buf);
    return ret;
}
#endif
