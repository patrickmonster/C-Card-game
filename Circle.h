#pragma once

#include "figure.h"

class Circle : public figure {
public:
	// ������ 
	Circle(int, int, int);

	//������
	virtual void draw(window *);

	//����Լ�
	int radius;
};