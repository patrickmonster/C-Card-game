#pragma once

#include "figure.h"

class rect : public figure {
public:
	// ������ �Լ�
	rect(int, int, int, int);
	
	// ����	
	int includes(const int, const int);
	virtual void draw(window *);

	//�������
	int width;
	int height;
};