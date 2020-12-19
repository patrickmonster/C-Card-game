#pragma once

#include "figure.h"

class rect : public figure {
public:
	// 생성자 함수
	rect(int, int, int, int);
	
	// 연산	
	int includes(const int, const int);
	virtual void draw(window *);

	//멤버변수
	int width;
	int height;
};