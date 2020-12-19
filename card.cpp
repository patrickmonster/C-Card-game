#include "card.h"

card::card(int xv, int yv, int s, int r)
	: rect(xv, yv, CardWidth, CardHeight)
{
	// then initialize the card fields
	suit = s;
	rank = r;
	faceUp = 1;
}
void card::flip()
{
	if (faceUp)
		faceUp = 0;
	else
		faceUp = 1;
}


void card::draw(window *win) {
	
	//draw the outer rect
	rect::draw(win);
	// then draw the inner contents
	if (faceUp) {
		// first print out the suit
		if (suit == 1 || suit == 4)
			win->setTextColor(brightRed);
		win->wout << setpos(x + 5, y + 5);

		if (suit == 1) win->wout << "¢¾";
		else if (suit == 2) win->wout << "¢À";
		else if (suit == 3) win->wout << "¢¼";
		else if (suit == 4) win->wout << "¡ß";
		// then print out the rank
		win->wout << setpos(x + 5, y + 20);
		if (rank == 1) win->wout << "ace";
		else if (rank == 11) win->wout << "jack";
		else if (rank == 12) win->wout << "queen";
		else if (rank == 13) win->wout << "king";
		else win->wout << rank;
		if (suit == 1 || suit == 4)
			win->setTextColor(black);
	}
	else {
		// face down, draw back
		win->wout << setpos(x + 30, y + 38);
		win->wout << "Back";
		win->line(x, y, x + CardWidth, y + CardHeight);
		win->line(x + CardWidth, y, x, y + CardHeight);
	}
	
}
