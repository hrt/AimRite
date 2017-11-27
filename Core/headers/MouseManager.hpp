#pragma once

#ifndef _MOUSE_MANAGER_H
#define _MOUSE_MANAGER_H

#include <stdlib.h>
#include <time.h>
#include "WindowManager.hpp"
#include "Vector2.hpp"

class MouseManager
{
private:
	int threshHold;
public:
	MouseManager();
	void executeMovementTo(WindowManager& wm, Vector2& to);
};

#endif