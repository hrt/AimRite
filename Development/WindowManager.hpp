#pragma once

#ifndef _WINDOW_MANAGER_H
#define _WINDOW_MANAGER_H

#include <Windows.h>
#include <string>
#include "Vector2.hpp"

class WindowManager
{
private:
	static HWND window;
	static BOOL CALLBACK SearchProc(HWND, LPARAM);

public:
	WindowManager();
	void FindWindow();
	bool WindowFocused();
	bool WindowFound();
	bool WindowExists();
	Vector2* GetWindowPosition();
};

#endif