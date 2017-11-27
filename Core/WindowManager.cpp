#include <stdio.h>
#include <iostream>

#include "headers/WindowManager.hpp"

HWND WindowManager::window = nullptr;

WindowManager::WindowManager() { }

void WindowManager::FindWindow()
{
	EnumWindows(SearchProc, 0);
	if (!WindowManager::window)
		Sleep(500);
}

bool WindowManager::WindowFound()
{
	return WindowManager::window != nullptr;
}

BOOL CALLBACK WindowManager::SearchProc(HWND hWnd, LPARAM lParam)
{
	static TCHAR buffer[50];

	GetWindowText(hWnd, buffer, 50);

	if (!strcmp("Battlerite", buffer))
	{
		WindowManager::window = hWnd;
		return FALSE;
	}

	return TRUE;
}

bool WindowManager::WindowFocused()
{
	HWND curr = GetForegroundWindow();

	if (!curr)
		return false;

	if (!WindowManager::window)
		return false;

	return (GetWindowThreadProcessId(curr, NULL) == GetWindowThreadProcessId(WindowManager::window, NULL));
}

bool WindowManager::WindowExists()
{
	return IsWindow(WindowManager::window);
}

Vector2* WindowManager::GetWindowPosition()
{
	RECT pos;
	GetWindowRect(WindowManager::window, &pos);

	Vector2* vec = new Vector2();
	vec->x = pos.left + 10;//possibly off a bit
	vec->y = pos.top;

	return vec;
}