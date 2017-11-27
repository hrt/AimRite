#pragma once

#ifndef _MOUSE_ORDER_H
#define _MOUSE_ORDER_H

class Vector2
{
public:
	Vector2()
	{
		single = true;
		left = true;

		x = -1;
		y = -1;
	}

	Vector2(int xval, int yval)
	{
		single = true;
		left = true;

		x = xval;
		y = yval;
	}

	Vector2(int xval, int yval, bool leftval)
	{
		single = true;
		left = leftval;

		x = xval;
		y = yval;
	}

	Vector2(Vector2* src)
	{
		this->x = src->x;
		this->y = src->y;

		this->single = src->single;
		this->left = src->left;
	}

public:
	int x, y;
	bool left;
	bool single;
};

#endif