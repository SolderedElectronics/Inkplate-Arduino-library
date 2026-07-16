/**
 **************************************************
 * @file        Gif.h
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

#ifndef __GIF_H__
#define __GIF_H__
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE13SPECTRA) &&            \
    !defined(ARDUINO_INKPLATE31SPECTRA)
#include "../../features/SdFat/SdFat.h"
#include "AnimatedGIF.h"
#include "WiFi.h"

class Inkplate;

/**
 * @brief       Gif class decodes and plays GIF images/animations on 1-bit
 *              Inkplate displays. Every pixel is converted with a fixed
 *              luminance threshold (RGB3BIT, no error-diffusion dithering) -
 *              dithering noise recomputed per frame reads as flicker between
 *              frames, so it is intentionally left out of the animation path.
 */
class Gif
{
  public:
    void begin(Inkplate *inkplateptr);

    // Draws only the first frame, e.g. to use a GIF as a static "poster" image.
    // Caller is responsible for calling display()/partialUpdate() afterwards,
    // same convention as Image::draw().
    bool drawGifFromBuffer(uint8_t *buf, int32_t len, int x, int y, bool invert = false);
    bool drawGifFromSd(const char *fileName, int x, int y, bool invert = false);
    bool drawGifFromWeb(const char *url, int x, int y, bool invert = false);

    // Plays the full animation, blocking until finished (or forever if loop == true).
    // fullRefreshEveryFrames is forwarded to Inkplate::setFullUpdateThreshold(): the
    // driver forces a full refresh every N partialUpdate() calls to clear partial-update
    // ghosting. Default of 60 matches the driver's own documented recommendation
    // (60-80) for best image quality; pass 0 to disable forced full refreshes.
    bool playGifFromBuffer(uint8_t *buf, int32_t len, int x, int y, bool invert = false, bool loop = true,
                           uint16_t fullRefreshEveryFrames = 60, bool leaveOn = true);
    bool playGifFromSd(const char *fileName, int x, int y, bool invert = false, bool loop = true,
                       uint16_t fullRefreshEveryFrames = 20, bool leaveOn = true);
    bool playGifFromWeb(const char *url, int x, int y, bool invert = false, bool loop = true,
                        uint16_t fullRefreshEveryFrames = 20, bool leaveOn = true);

    // Call from a button press / ISR-set flag to break out of a running playGif*() call.
    void stop();

  private:
    struct GifDrawContext
    {
        Gif *self;
        int drawX, drawY;
        bool invert;
    };

    Inkplate *_inkplate = NULL;
    AnimatedGIF _gif;
    volatile bool _stopRequested = false;

    static void gifDrawCallback(GIFDRAW *pDraw);
};

#endif
#endif
