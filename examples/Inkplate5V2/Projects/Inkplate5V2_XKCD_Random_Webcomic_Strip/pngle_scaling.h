/*
   pngle_scaling.h - PNG scaling functionality
   
   Based on pngle library (https://github.com/kikuchan/pngle)
   Added: scaling with aspect ratio preservation, centering, grayscale conversion
*/

#ifndef PNGLE_SCALING_H
#define PNGLE_SCALING_H

#include "Inkplate.h"

// Forward declarations for pngle types and functions (to avoid include path issues)
extern "C" {
    typedef struct pngle pngle_t;
    typedef struct {
        uint32_t width;
        uint32_t height;
        uint8_t depth;
        uint8_t color_type;
    } pngle_ihdr_t;
    
    typedef void (*pngle_draw_callback_t)(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]);
    
    pngle_t* pngle_new();
    void pngle_destroy(pngle_t *pngle);
    int pngle_feed(pngle_t *pngle, const void *buf, size_t len);
    void pngle_set_draw_callback(pngle_t *pngle, pngle_draw_callback_t callback);
    pngle_ihdr_t* pngle_get_ihdr(pngle_t *pngle);
    uint32_t pngle_get_width(pngle_t *pngle);
    uint32_t pngle_get_height(pngle_t *pngle);
}

// Scaling parameters structure
struct ScalingContext {
    float scaleX;
    float scaleY;
    uint16_t targetWidth;
    uint16_t targetHeight;
    uint16_t sourceWidth;
    uint16_t sourceHeight;
    uint16_t offsetX;
    uint16_t offsetY;
    bool dither;
    bool invert;
    Inkplate* displayPtr;
};

// Global scaling context - declared in pngle_scaling.cpp
extern ScalingContext scalingCtx;

// Reference to display object - must be set by the application
extern Inkplate display;

// Function declarations
void pngle_on_draw_scaled(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]);
bool drawScaledPngFromBuffer(const uint8_t *buffer, int32_t size, uint16_t targetWidth, uint16_t targetHeight, bool dither = false, bool invert = false);

#endif // PNGLE_SCALING_H