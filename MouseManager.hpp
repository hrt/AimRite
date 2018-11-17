#pragma once

#ifndef _MOUSE_MANAGER_H
#define _MOUSE_MANAGER_H

#include "WindowManager.hpp"
#include "Vector2.hpp"
//
#define SCREEN_WIDTH 1680
#define SCREEN_HEIGHT 1050
//#define SCREEN_WIDTH 2560
//#define SCREEN_HEIGHT 1440
#define SCALE 95

class MouseManager
{
private:
	int threshHold;
public:
	MouseManager();
	void executeMovementTo(WindowManager& wm, Vector2& to);
};

#endif