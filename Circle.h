#pragma once

#include "figure.h"

class Circle : public figure {
public:
	// 생성자 
	Circle(int, int, int);

	//연산자
	virtual void draw(window *);

	//멤버함수
	int radius;
};