#include "rect.h"

rect::rect(int xv, int yv, int wid, int hgt) : figure(xv, yv)
{
	width = wid;
	height = hgt;
}



void rect::draw(window * theWindow)
{
	
	// first make sure the pen is correct
	theWindow->setPen(blue, solidLine, 1);

	// then draw the lines
	theWindow->line(x, y, x + width, y);
	theWindow->line(x, y, x, y + height);
	theWindow->line(x, y + height, x + width, y + height);
	theWindow->line(x+width, y, x + width, y + height);	
}

int rect::includes(int xv, int yv)
{
	
	if ((x < xv) && (x+width > xv))
		if ((y < yv) && (y+height > yv)) 	
			return 1;
		
	return 0;
	
}
