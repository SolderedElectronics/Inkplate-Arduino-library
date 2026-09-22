/*
   pngle_scaling.cpp - PNG scaling implementation

   Based on pngle library (https://github.com/kikuchan/pngle)
   Added: scaling with aspect ratio preservation, centering, grayscale conversion

   This module handles ONLY PNG decoding and scaling from a pre-downloaded buffer.
   Networking/download concerns are handled by the main sketch.
*/

#include "pngle_scaling.h"

// Global scaling context definition
ScalingContext scalingCtx;

// Custom PNG drawing callback with scaling
void pngle_on_draw_scaled(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
    if (rgba[3]) // Only process non-transparent pixels
    {
        // Process the RGBA values once for this rectangular region
        uint8_t r = rgba[0];
        uint8_t g = rgba[1];
        uint8_t b = rgba[2];

        // Convert to grayscale
        uint8_t gray = (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);

        // Apply inversion if needed
        if (scalingCtx.invert) {
            gray = 255 - gray;
        }

        // Convert to appropriate bit depth for current display mode
        uint8_t px;
        if (scalingCtx.displayPtr->getDisplayMode() == INKPLATE_3BIT) {
            // For 3-bit mode: scale from 0-255 to 0-7
            px = (gray * 7) / 255;
        } else {
            // For 1-bit mode: convert to black/white
            px = (gray > 128) ? 0 : 1; // 0=white, 1=black
        }
        
        // Now iterate through ALL pixels in this rectangular region
        for (int j = 0; j < h; ++j)
        {
            for (int i = 0; i < w; ++i)
            {
                // Calculate source pixel position
                uint32_t srcX = x + i;
                uint32_t srcY = y + j;
                
                // Calculate the target rectangle this source pixel should fill
                uint32_t scaledXStart = (uint32_t)(srcX * scalingCtx.scaleX);
                uint32_t scaledYStart = (uint32_t)(srcY * scalingCtx.scaleY);
                uint32_t scaledXEnd = (uint32_t)((srcX + 1) * scalingCtx.scaleX);
                uint32_t scaledYEnd = (uint32_t)((srcY + 1) * scalingCtx.scaleY);
                
                // Apply offset for centering
                scaledXStart += scalingCtx.offsetX;
                scaledYStart += scalingCtx.offsetY;
                scaledXEnd += scalingCtx.offsetX;
                scaledYEnd += scalingCtx.offsetY;
                
                // Fill the entire rectangular area for this source pixel
                for (uint32_t targetY = scaledYStart; targetY < scaledYEnd && targetY < scalingCtx.displayPtr->height(); targetY++) {
                    for (uint32_t targetX = scaledXStart; targetX < scaledXEnd && targetX < scalingCtx.displayPtr->width(); targetX++) {
                        scalingCtx.displayPtr->drawPixel(targetX, targetY, px);
                    }
                }
            }
        }
    }
}

bool drawScaledPngFromBuffer(const uint8_t *buffer, int32_t size, uint16_t targetWidth, uint16_t targetHeight, bool dither, bool invert)
{
    // Initialize PNG decoder
    pngle_t *pngle = pngle_new();
    if (!pngle) {
        Serial.println("Failed to create PNG decoder");
        return false;
    }

    // First pass: decode without callback to get dimensions
    Serial.printf("[PNG] First pass: feeding %d bytes to decoder\n", size);
    int fed = pngle_feed(pngle, buffer, size);
    if (fed < 0) {
        Serial.printf("[PNG] ERROR: Decode failed with error code %d\n", fed);
        Serial.printf("[PNG] Buffer size: %d bytes, fed: %d bytes\n", size, fed);
        Serial.printf("[PNG] First 8 bytes: %02X %02X %02X %02X %02X %02X %02X %02X\n",
                     buffer[0], buffer[1], buffer[2], buffer[3],
                     buffer[4], buffer[5], buffer[6], buffer[7]);
        pngle_destroy(pngle);
        return false;
    }
    Serial.printf("[PNG] First pass successful, fed %d bytes\n", fed);

    // Get image dimensions
    pngle_ihdr_t *ihdr = pngle_get_ihdr(pngle);
    if (!ihdr) {
        Serial.println("[PNG] ERROR: Failed to get PNG header");
        pngle_destroy(pngle);
        return false;
    }
    Serial.printf("[PNG] Header: %dx%d, bit depth: %d, color type: %d\n",
                 ihdr->width, ihdr->height, ihdr->depth, ihdr->color_type);
    
    uint32_t sourceWidth = ihdr->width;
    uint32_t sourceHeight = ihdr->height;
    
    Serial.printf("Source image: %dx%d\n", sourceWidth, sourceHeight);
    Serial.printf("Target size: %dx%d\n", targetWidth, targetHeight);
    
    // Calculate scaling parameters
    float scaleX = (float)targetWidth / sourceWidth;
    float scaleY = (float)targetHeight / sourceHeight;
    float scale = min(scaleX, scaleY); // Maintain aspect ratio
    
    uint16_t scaledWidth = (uint16_t)(sourceWidth * scale);
    uint16_t scaledHeight = (uint16_t)(sourceHeight * scale);
    
    // Setup scaling context
    scalingCtx.scaleX = scale;
    scalingCtx.scaleY = scale;
    scalingCtx.targetWidth = scaledWidth;
    scalingCtx.targetHeight = scaledHeight;
    scalingCtx.sourceWidth = sourceWidth;
    scalingCtx.sourceHeight = sourceHeight;
    scalingCtx.offsetX = (targetWidth - scaledWidth) / 2;
    scalingCtx.offsetY = (targetHeight - scaledHeight) / 2;
    scalingCtx.dither = dither;
    scalingCtx.invert = invert;
    scalingCtx.displayPtr = &display;
    
    Serial.printf("Scaling factor: %.2f\n", scale);
    Serial.printf("Scaled size: %dx%d\n", scaledWidth, scaledHeight);
    Serial.printf("Offset: %d,%d\n", scalingCtx.offsetX, scalingCtx.offsetY);
    
    // Clear display and create new decoder for actual drawing
    scalingCtx.displayPtr->clearDisplay();
    pngle_destroy(pngle);
    
    // Second pass: decode with scaling callback
    Serial.println("[PNG] Starting second pass with scaling...");
    pngle = pngle_new();
    if (!pngle) {
        Serial.println("[PNG] ERROR: Failed to create second PNG decoder");
        return false;
    }

    // Set our custom drawing callback for the second pass
    pngle_set_draw_callback(pngle, pngle_on_draw_scaled);

    // Process the entire PNG data with scaling
    int fed2 = pngle_feed(pngle, buffer, size);
    if (fed2 < 0) {
        Serial.printf("[PNG] ERROR: Decode failed in scaling pass with error code %d\n", fed2);
        pngle_destroy(pngle);
        return false;
    }

    Serial.printf("[PNG] Second pass successful, rendering complete\n");
    pngle_destroy(pngle);

    return true;
}