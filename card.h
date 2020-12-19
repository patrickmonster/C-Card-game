#include "rect.h"

const int CardWidth = 75;
const int CardHeight = 100;
class card : public rect {
public:
	// constructor
	card(int, int, int, int);
	// operations
	virtual void draw(window *);
	void flip();

	// data fields
	int suit;
	int rank;
	int faceUp;

};
