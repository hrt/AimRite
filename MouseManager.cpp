#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <string.h>
#include <windows.h>
#include "MouseManager.hpp"


INPUT buffer;

void mouseSetup()
{
	buffer.type = INPUT_MOUSE;
	buffer.mi.dx = (0 * (0xFFFF / SCREEN_WIDTH));
	buffer.mi.dy = (0 * (0xFFFF / SCREEN_HEIGHT));
	buffer.mi.mouseData = 0;
	buffer.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
	buffer.mi.time = 0;
	buffer.mi.dwExtraInfo = 0;
}

void mouseMoveAbs(int x, int y)
{
	buffer.mi.dx = (x * (0xFFFF / SCREEN_WIDTH));
	buffer.mi.dy = (y * (0xFFFF / SCREEN_HEIGHT));
	buffer.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);

	SendInput(1, &buffer, sizeof(INPUT));
}


MouseManager::MouseManager()
{
	mouseSetup();
}

void MouseManager::executeMovementTo(WindowManager& wm, Vector2& to)
{
	Vector2* vec = wm.GetWindowPosition();
	mouseMoveAbs(vec->x + to.x, vec->y + to.y);
	delete vec;
}
