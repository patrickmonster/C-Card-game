#include "Circle.h"

Circle::Circle(int xv, int yv, int r) : figure(xv, yv) {
	radius = r;
}

void Circle::draw(window * theWindow) {
	theWindow->setPen(blue, solidLine, 1);
	theWindow->circle(x, y, radius);
}