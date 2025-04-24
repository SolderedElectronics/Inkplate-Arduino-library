/**
 **************************************************
 * @file        Graphics.cpp
 * @brief       Basic graphics functionalities
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

#include "Graphics.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                                                            \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

/**
 * @brief       setRotation function sets _width and _height modified by current
 * rotation
 *
 * @param       uint8_t x
 *              screen rotation 0 is normal, 1 is left, 2 is upsidedown and 3 is
 * right
 */
void Graphics::setRotation(uint8_t x)
{
    rotation = (x & 3);
    switch (rotation)
    {
    case 0:
    case 2:
        _width = WIDTH;
        _height = HEIGHT;
        break;
    case 1:
    case 3:
        _width = HEIGHT;
        _height = WIDTH;
        break;
    }
}

/**
 * @brief       getRotation gets screen rotation
 *
 * @return      0 is normal, 1 is left, 2 is upsidedown and 3 is right
 */
uint8_t Graphics::getRotation()
{
    return rotation;
}

/**
 * @brief       drawPixes function that calls drawPixes for different screen
 * sizes
 *
 * @param       int16_t x0
 *              x position, will change depending on rotation
 * @param       int16_t y0
 *              y position, will change depending on rotation
 *
 * @param       uint16_t color
 *              pixel color, in 3bit mode have values in range 0-7
 */
void Graphics::drawPixel(int16_t x0, int16_t y0, uint16_t color)
{
    writePixel(x0, y0, color); // Specified in boards folder
}

void Graphics::startWrite()
{
}

/**
 * @brief       writeFillRectangle function writes filled rectangle starting at
 * x,y position
 *
 * @param       int16_t x
 *              upper left corner x position for rectangle
 * @param       int16_t y
 *              upper right corner y position for rectangle
 * @param       int16_t w
 *              rectangle width
 * @param       int16_t h
 *              rectangle height
 * @param       uint16_t color
 *              pixel color, in 3bit mode have values in range 0-7
 */
void Graphics::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            writePixel(x + j, y + i, color);
}

/**
 * @brief       writeFastVLine function writes vertical line starting at x,y
 * position
 *
 * @param       int16_t x
 *              starting x position for vertical line
 * @param       int16_t y
 *              starting y position for vertical line
 * @param       int16_t h
 *              vertical line height
 * @param       uint16_t color
 *              pixel color, in 3bit mode have values in range 0-7
 */
void Graphics::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    for (int i = 0; i < h; ++i)
        writePixel(x, y + i, color);
}

/**
 * @brief       writeFastHLine function writes horizontal line starting at x,y
 * position
 *
 * @param       int16_t x
 *              starting x position for horizontal line
 * @param       int16_t y
 *              starting y position for horizontal line
 * @param       int16_t w
 *              horizontal line width
 * @param       uint16_t color
 *              pixel color, in 3bit mode have values in range 0-7
 */
void Graphics::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    for (int j = 0; j < w; ++j)
        writePixel(x + j, y, color);
}

/**
 * @brief       writeLine function that writes line at the degree
 *
 * @param       int16_t x0
 *              starting x position for line
 * @param       int16_t y0
 *              starting y position for line
 * @param       int16_t x1
 *              ending x position for line
 * @param       int16_t y1
 *              ending y position for line
 * @param       uint16_t color
 *              pixel color, in 3bit mode have values in range 0-7
 */
void Graphics::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1)
    {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx >> 1;
    int16_t ystep;

    if (y0 < y1)
        ystep = 1;
    else
        ystep = -1;

    for (; x0 <= x1; x0++)
    {
        if (steep)
            writePixel(y0, x0, color);
        else
            writePixel(x0, y0, color);
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

void Graphics::endWrite()
{
}

#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE4) &&                   \
    !defined(ARDUINO_INKPLATE7)

/**
 * @brief       setDisplayMode sets display mode
 *
 * @param       uint8_t _mode
 *
 * @note        can't be used with color displays
 */
void Graphics::setDisplayMode(uint8_t _mode)
{
    _displayMode = _mode;
}

/**
 * @brief       getDisplayMode gets display mode
 *
 * @return      0 for black and white, 1 for grayscale
 */
uint8_t Graphics::getDisplayMode()
{
    return _displayMode;
}

/**
 * @brief       selectDisplayMode selects display mode and sets memory
 * allocation  for display buffers
 *
 * @param       uint8_t _mode
 *              display mode 0 for black and white, 1 for grayscale
 */
void Graphics::selectDisplayMode(uint8_t _mode)
{
    if (_mode != _displayMode)
    {
        _displayMode = _mode & 1;
        memset(DMemoryNew, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
        memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
        memset(_pBuffer, 0, E_INK_WIDTH * E_INK_HEIGHT / 4);
        memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);
    }
}
#endif

/**
 * @brief       width function returns screen width
 *
 * @return      screen width
 */
int16_t Graphics::width()
{
    return _width;
};

/**
 * @brief       width function returns screen height
 *
 * @return      screen height
 */
int16_t Graphics::height()
{
    return _height;
};


/**
 * @brief       Draws a text box with optional border and word-wrapped content
 *
 * @param       int16_t x0  
 *              Top-left x-coordinate of the text box
 * @param       int16_t y0  
 *              Top-left y-coordinate of the text box
 * @param       int16_t x1  
 *              Bottom-right x-coordinate of the text box
 * @param       int16_t y1  
 *              Bottom-right y-coordinate of the text box
 * @param       const char* text  
 *              The null-terminated string to be rendered inside the box
 * @param       uint16_t textSizeMultiplier  
 *              Size multiplier for the text
 * @param       const GFXfont* font  
 *              Pointer to the font to use for rendering the text
 * @param       uint16_t verticalSpacing  
 *              Vertical spacing (in pixels) between lines of text; if 0 or NULL, defaults to text height + padding
 * @param       bool showBorder  
 *              Whether to draw a border around the text box
 * @param       uint16_t fontSize  
 *              Font size in points (pt)
 *
 * @details     This function renders a block of text inside the defined rectangular area.
 *              It automatically wraps words to the next line if they exceed the available width.
 *              If the text does not fit entirely, an ellipsis ("...") is added to the final visible line.
 *              Text is padded with spaces to keep a consistent line length.
 */
void Graphics::drawTextBox(int16_t x0,int16_t y0,int16_t x1,int16_t y1, const char* text,uint16_t textSizeMultiplier, const GFXfont *font, uint16_t verticalSpacing, bool showBorder, uint16_t fontSize )
{
    int16_t currentX = x0;
    int16_t currentY = y0;

    int16_t textLenght=strlen(text);
    int offset=0;
    fontSize=(fontSize*3)/4; //1pt = 4/3 px
    int numOfCharactersPerLine=(x1-x0)/(textSizeMultiplier*fontSize);
    int16_t currentLineLenght=numOfCharactersPerLine;
    this->setTextSize(textSizeMultiplier);
    this->setFont(font);
    if(showBorder)
    {
        this->drawRect(x0,y0,(x1-x0),(y1-y0),1);
    }
    if(verticalSpacing==NULL)
    {
        verticalSpacing=textSizeMultiplier*fontSize+6;
    }
    for (int i = y0; i < (y1 - verticalSpacing); i += verticalSpacing)
    {
        currentY = i;
        this->setCursor(currentX, currentY);
    
        int remainingLength = textLenght - offset;
        int lineLength = (remainingLength < currentLineLenght) ? remainingLength : currentLineLenght;
    
        // Temporary buffer to hold potential line
        char* buffer = (char*)malloc((lineLength + 1) * sizeof(char));
        memcpy(buffer, text + offset, lineLength);
        buffer[lineLength] = '\0';
    
        // Find the last space in buffer to wrap at word boundary
        int lastSpaceIndex = -1;
        for (int j = 0; j < lineLength; ++j) {
            if (buffer[j] == ' ') lastSpaceIndex = j;
        }
    
        // If a word gets cut, wrap to the next line
        if ((offset + lineLength < textLenght) && (text[offset + lineLength] != ' ') && (lastSpaceIndex != -1) && ((i + verticalSpacing) < (y1 - verticalSpacing))) {
            lineLength = lastSpaceIndex + 1; // Include the space
        }
    
        // Allocate space for actual line with null-terminator
        char* textPart = (char*)malloc((currentLineLenght + 1) * sizeof(char));
        memset(textPart, ' ', currentLineLenght); // Fill with spaces
        memcpy(textPart, text + offset, lineLength); // Copy valid part
        textPart[currentLineLenght] = '\0';
    
        // Ellipsis on final visible line
        if ((i + verticalSpacing) >= (y1 - verticalSpacing) && (offset + lineLength < textLenght)) {


            textPart[currentLineLenght - 1] = '.';
            textPart[currentLineLenght - 2] = '.';
            textPart[currentLineLenght - 3] = '.';
        }
    
        this->print(textPart);
    
        offset += lineLength;
        free(buffer);
        free(textPart);
    
        if (offset >= textLenght) return;
    }
    

}