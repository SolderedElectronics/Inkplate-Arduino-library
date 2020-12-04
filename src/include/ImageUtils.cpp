#include "Image.h"

void Image::getPointsForPosition(const Position& position, const uint16_t imageWidth, const uint16_t imageHeight, 
	const uint16_t screenWidth, const uint16_t screenHeight, uint16_t *posX, uint16_t *posY)
{
	*posX = 0;
	*posY = 0;

	switch(position) {
		case TopLeft:
			break;
		case Center:
			if(imageWidth < screenWidth)
				*posX = (screenWidth - imageWidth) >> 1; 
			
			if(imageHeight < screenHeight)
				*posY = (screenHeight - imageHeight) >> 1;	
			break;
		case BottomLeft:
			if(imageHeight < screenHeight)
				*posY = screenHeight - imageHeight;
			break;
		case TopRight:
			if(imageWidth < screenWidth)
				*posX = screenWidth - imageWidth; 
			break;
		case BottomRight:
			if(imageWidth < screenWidth)
				*posX = screenWidth - imageWidth;
			if(imageHeight < screenHeight)
				*posY = screenHeight - imageHeight;
			break;
		default:
			break;
	}
}
