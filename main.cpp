#include "pclaf.h"
#include "rect.h"
#include "card.h"
#include "Circle.h"

class cardApplication : public application {
public:
	cardApplication(char *);
	cardApplication(char *, int, int, int, int);
	void mouseDown(int, int);
	void mouseRDown(int, int);
	void paint();
	
};

const int MAXFIGURES = 20;

figure * figures[MAXFIGURES] = { 0 };

cardApplication::cardApplication(char * apName) :application(apName)
{
	// no action
}

cardApplication::cardApplication(char * apName, int width, int height, int x, int y) : application(apName, width, height, x, y)
{
	// no action
}


void cardApplication::mouseDown(int x, int y)
{	
	int i;
	for (i = 0; figures[i] != 0; i++)
	{ 	}
	
	figures[i] = new card(x, y, 2, 7);

	update();
}

void cardApplication::mouseRDown(int x, int y)
{
	int i;
	for (i = 0; figures[i] != 0; i++)
	{
	}

	figures[i] = new Circle(x, y, 40);
	update();
}

void cardApplication::paint()
{
	for (int i = 0; figures[i] != 0; i++) {
		figures[i]->draw(this);
	}
}

int mainLAF(void)
{
	cardApplication theApp("Card");
	
	theApp.run();
	return 0;
}


