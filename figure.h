#pragma once
#include "pclaf.h"

class figure {
public:

	figure(int, int);

	//����
	void moveTo(int, int);
	virtual void draw(window *) = 0;

	//�������
	int x;
	int y;
};
