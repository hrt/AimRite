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
	MouseManager mouse;

	std::cout << "Searching for window.." << std::endl;
	while (!window.WindowFound())
	{
		window.FindWindow();
	}
	std::cout << "Window found!" << std::endl << std::endl;

	PlayerInformation playerInformation[20];

	while (window.WindowFocused() || window.WindowExists())
	{
		// Get local players coordinates
		DWORD p1 = Read<DWORD>(memory.handle, memory.Battlerite_Base + OFFSET_LOCAL_PLAYER[0]);
		DWORD p2 = Read<DWORD>(memory.handle, p1 + OFFSET_LOCAL_PLAYER[1]);
		DWORD p3 = Read<DWORD>(memory.handle, p2 + OFFSET_LOCAL_PLAYER[2]);
		DWORD p4 = Read<DWORD>(memory.handle, p3 + OFFSET_LOCAL_PLAYER[3]);
		DWORD p5 = Read<DWORD>(memory.handle, p4 + OFFSET_LOCAL_PLAYER[4]);

		float x = Read<float>(memory.handle, p5 + OFFSET_LOCAL_X);
		float y = Read<float>(memory.handle, p5 + OFFSET_LOCAL_Y);

		// Get entity list
		DWORD m1 = Read<DWORD>(memory.handle, memory.MonoDll_Base + OFFSET_ENTITY_LIST[0]);
		DWORD m2 = Read<DWORD>(memory.handle, m1 + OFFSET_ENTITY_LIST[1]);
		DWORD m3 = Read<DWORD>(memory.handle, m2 + OFFSET_ENTITY_LIST[2]);
		DWORD m4 = Read<DWORD>(memory.handle, m3 + OFFSET_ENTITY_LIST[3]);

		// Find closest player for teams 1 and 2 respectively
		float closest1 = 1000000000.f;
		float closest2 = 1000000000.f;
		int closest1Index = -1;
		int closest2Index = -1;

		// Local players team
		int playerTeam = -1;

		for (int i = 0; i < 6; i++)
		{
			float targetX = Read<float>(memory.handle, m4 + OFFSET_PLAYER_START + OFFSET_PLAYER_X + i * PLAYER_SIZE);
			int targetTeam = Read<int>(memory.handle, m4 + OFFSET_PLAYER_START + OFFSET_PLAYER_TEAM + i * PLAYER_SIZE);
			float targetY = Read<float>(memory.handle, m4 + OFFSET_PLAYER_START + OFFSET_PLAYER_Y + i * PLAYER_SIZE);

			// Distance to target
			float dx = x - targetX;
			float dy = y - targetY;
			float distanceToTarget = dx * dx + dy * dy;

			playerInformation[i].speedX = targetX - playerInformation[i].x;
			playerInformation[i].speedY = targetY - playerInformation[i].y;

			// Filter anomalous results, huge change in coordinates -> ignore
			if (abs(playerInformation[i].speedX) > 11 || abs(playerInformation[i].speedY) > 11)
			{
				playerInformation[i].previousX = playerInformation[i].x;
				playerInformation[i].previousY = playerInformation[i].y;
				playerInformation[i].x = targetX;
				playerInformation[i].y = targetY;
				continue;
			}

			// Ignore dead people or afk people
			if (abs(playerInformation[i].speedX) < 0.01f
				&& abs(playerInformation[i].speedY) < 0.01f)
			{
				// Dead or not moving for 1s
				if (++playerInformation[i].idle > 20)
					continue;
			}
			else
			{
				// Reset afk timer
				playerInformation[i].idle = 0;
			}

			// Ignore entities that are really far away
			if (distanceToTarget > 1000.f)
			{
				continue;
			}

			// Update closest target information
			if (distanceToTarget < 1.f)
			{
				// Local player
				playerTeam = targetTeam;
			}
			else if (targetTeam == 1 && distanceToTarget < closest1)
			{
				closest1 = distanceToTarget;
				closest1Index = i;
			}
			else if (targetTeam == 2 && distanceToTarget < closest2)
			{
				closest2 = distanceToTarget;
				closest2Index = i;
			}
			else
			{
				// Ignore teams other than 1 and 2
				continue;
			}

			playerInformation[i].previousX = playerInformation[i].x;
			playerInformation[i].previousY = playerInformation[i].y;
			playerInformation[i].x = targetX;
			playerInformation[i].y = targetY;
		}

		// Pick which closest player you want to target (ally or enemy)
		float distanceToTarget = -1.f;
		PlayerInformation targetPlayer;

		if (playerTeam == 2)
		{
			if (closest1Index == -1)
			{
				continue;
			}
			 distanceToTarget = closest1;
			targetPlayer = playerInformation[closest1Index];
		}
		else if (playerTeam == 1)
		{
			if (closest2Index == -1)
			{
				continue;
			}
			distanceToTarget = closest2;
			targetPlayer = playerInformation[closest2Index];
		}
		else
		{
			// Local player is not on a team, may not be in game or dead so do not move mouse
			continue;
		}

		// If mouse button 5 is not pressed then aim at closest target
		if (distanceToTarget > 1.f && !((GetKeyState(VK_XBUTTON2) & 0x100) != 0))
		{
			// Movement prediction
			float dx = targetPlayer.x + targetPlayer.speedX*3 - x;
			float dy = targetPlayer.y + targetPlayer.speedY*3 - y;

			Vector2* vec = window.GetWindowPosition();

			// Screen is flipped for team 2
			if (playerTeam == 2)
			{
				dx *= -1;
				dy *= -1;
			}

			// To be as accurate as possible, aim at the edge of the screen that should aim through the target
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


			mouse.executeMovementTo(window, *vec);
			Sleep(50);
		}
	}

	return EXIT_SUCCESS;
}
