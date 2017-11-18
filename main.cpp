#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "WindowManager.hpp"
#include "MouseManager.hpp"
#include "MemoryManager.h"
#include "Offsets.hpp"

#include <windows.h>
#include <tlhelp32.h>

template<class c>
c Read(HANDLE processHandle, DWORD dwAddress)
{
	c val;
	ReadProcessMemory(processHandle, (LPVOID)dwAddress, &val, sizeof(c), NULL);
	return val;
}

int main(int argc, char** argv) {
	MemoryManager memory;
	WindowManager window;

	std::cout << "Searching for window.." << std::endl;
	while (!window.WindowFound())
	{
		window.FindWindow();
	}
	std::cout << "Window found!" << std::endl << std::endl;

	while (window.WindowFocused() || window.WindowExists())
	{
		DWORD p1 = Read<DWORD>(memory.handle, memory.Battlerite_Base + OFFSET_LOCAL_PLAYER[0]);
		DWORD p2 = Read<DWORD>(memory.handle, p1 + OFFSET_LOCAL_PLAYER[1]);
		DWORD p3 = Read<DWORD>(memory.handle, p2 + OFFSET_LOCAL_PLAYER[2]);
		DWORD p4 = Read<DWORD>(memory.handle, p3 + OFFSET_LOCAL_PLAYER[3]);
		DWORD p5 = Read<DWORD>(memory.handle, p4 + OFFSET_LOCAL_PLAYER[4]);
		float x = Read<float>(memory.handle, p5 + OFFSET_LOCAL_X);
		float y = Read<float>(memory.handle, p5 + OFFSET_LOCAL_Y);


		DWORD m1 = Read<DWORD>(memory.handle, memory.MonoDll_Base + OFFSET_ENTITY_LIST[0]);
		DWORD m2 = Read<DWORD>(memory.handle, m1 + OFFSET_ENTITY_LIST[1]);
		DWORD m3 = Read<DWORD>(memory.handle, m2 + OFFSET_ENTITY_LIST[2]);
		DWORD m4 = Read<DWORD>(memory.handle, m3 + OFFSET_ENTITY_LIST[3]);

		float closest1 = 1000000000.f;
		float closest2 = 1000000000.f;
		int playerTeam = -1;

		float closest1x = -1.f;
		float closest1y = -1.f;
		float closest2x = -1.f;
		float closest2y = -1.f;

		for (int i = 0; i < 5; i++)
		{
			float targetX = Read<float>(memory.handle, m4 + OFFSET_PLAYER_X + i * PLAYER_SIZE);
			int targetTeam = Read<int>(memory.handle, m4 + OFFSET_PLAYER_TEAM + i * PLAYER_SIZE);
			float targetY = Read<float>(memory.handle, m4 + OFFSET_PLAYER_Y + i * PLAYER_SIZE);

			float dx = x - targetX;
			float dy = y - targetY;
			float distanceToTarget = dx * dx + dy * dy;


			if (distanceToTarget < 1.f)
			{
				playerTeam = targetTeam;
			}
			else if (targetTeam == 1 && distanceToTarget < closest1)
			{
				closest1 = distanceToTarget;
				closest1x = targetX;
				closest1y = targetY;
			}
			else if (targetTeam == 2 && distanceToTarget < closest2)
			{
				closest2 = distanceToTarget;
				closest2x = targetX;
				closest2y = targetY;
			}
		}

		float distanceToTarget = closest1;
		float targetX = closest1x;
		float targetY = closest1y;
		if (playerTeam != 2)
		{
			distanceToTarget = closest2;
			targetX = closest2x;
			targetY = closest2y;
		}
		else if (playerTeam == -1)
		{
			continue;
		}


		if (distanceToTarget > 1.f && !((GetKeyState(VK_XBUTTON2) & 0x100) != 0))
		{
			// todo : range check
			float dx = targetX - x;
			float dy = targetY - y;

			std::cout << "Me : " << x << " " << y << std::endl;
			std::cout << "En : " << targetX << " " << targetY << std::endl;
			std::cout << "XX : " << dx << " " << dy << std::endl;

			Vector2* vec = window.GetWindowPosition();

			if (playerTeam == 2)
			{
				dx *= -1;
				dy *= -1;
			}

			if (abs(dy) > abs(dx))
			{
				float multiplier = abs((1080 / 2) / dy);

				if (dy < 0)
				{
					vec->y = 1080;
				}
				else
				{
					vec->y = 0;
				}
				vec->x = 1920 / 2 + dx * multiplier;
			}
			else
			{
				float multiplier = abs((1920 / 2) / dx);

				if (dx > 0)
				{
					vec->x = 1920;
				}
				else
				{
					vec->x = 0;
				}
				vec->y = 1080 / 2 - dy * multiplier;
			}


			MouseManager mouse(2);
			mouse.executeMovementTo(window, *vec);
			Sleep(50);
		}
	}

	return EXIT_SUCCESS;
}
