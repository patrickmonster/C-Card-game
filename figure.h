#pragma once
#include "pclaf.h"

class figure {
public:

	figure(int, int);

	//연산
	void moveTo(int, int);
	virtual void draw(window *) = 0;

	//멤버변수
	int x;
	int y;
};
