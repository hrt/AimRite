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

		// Find closest player for allies and enemies
		float closest1 = 1000000000.f;
		float closest2 = 1000000000.f;
		int closest1Index = -1;
		int closest2Index = -1;

		// Local players team
		int playerTeam = -1;

		// Loop through entities
		for (int i = 0; i < 7; i++)
		{
			float targetX = Read<float>(memory.handle, m4 + OFFSET_ENTITY_START + OFFSET_ENTITY_X + i * PLAYER_SIZE);
			int targetTeam = Read<int>(memory.handle, m4 + OFFSET_ENTITY_START + OFFSET_ENTITY_TEAM + i * PLAYER_SIZE);
			float targetY = Read<float>(memory.handle, m4 + OFFSET_ENTITY_START + OFFSET_ENTITY_Y + i * PLAYER_SIZE);
			float targetDirection = Read<float>(memory.handle, m4 + OFFSET_ENTITY_START + OFFSET_ENTITY_DIRECTION + i * PLAYER_SIZE);

			// Ignore other teams
			if (targetTeam != 1 && targetTeam != 2)
				continue;

			// Ignore skill shots
			if (targetDirection)
				continue;

			// Distance to target
			float dx = x - targetX;
			float dy = y - targetY;
			float distanceToTarget = dx * dx + dy * dy;

			playerInformation[i].velocityX = targetX - playerInformation[i].x;
			playerInformation[i].velocityY = targetY - playerInformation[i].y;

			// Ignore dead people or afk people (1 seconds)
			if (abs(playerInformation[i].velocityX) > 0.1f
				|| abs(playerInformation[i].velocityY) > 0.1f)
			{
				// Update timer
				playerInformation[i].lastUpdate = clock();
			}

			float differenceInTime = (clock() - playerInformation[i].lastUpdate) / CLOCKS_PER_SEC;

			if (differenceInTime > 1.f)
				continue;

			// Ignore entities that are really far away
			if (distanceToTarget > 1000.f)
				continue;

			if (distanceToTarget < 1.f)
			{
				// Local player found
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

			playerInformation[i].previousX = playerInformation[i].x;
			playerInformation[i].previousY = playerInformation[i].y;
			playerInformation[i].x = targetX;
			playerInformation[i].y = targetY;
		}

		// Pick which closest player you want to target (ally or enemy)
		float distanceToEnemy = -1.f;
		float distanceToAlly = -1.f;
		PlayerInformation targetEnemy;
		PlayerInformation targetAlly;


		if (playerTeam == 2)
		{
			if (closest1Index != -1)
			{
				distanceToEnemy = closest1;
				targetEnemy = playerInformation[closest1Index];
			}

			if (closest2Index != -1)
			{
				distanceToAlly = closest2;
				targetAlly = playerInformation[closest2Index];
			}
		}
		else if (playerTeam == 1)
		{
			if (closest1Index != -1)
			{
				distanceToAlly = closest1;
				targetAlly = playerInformation[closest1Index];
			}

			if (closest2Index != -1)
			{
				distanceToEnemy = closest2;
				targetEnemy = playerInformation[closest2Index];
			}
		}
		else
		{
			// Local player is not on a team, may not be in game or dead so do not move mouse
			continue;
		}

		// The aimbot
		// If mouse button 5 is not pressed then aim at closest target
		if (distanceToEnemy > 1.f && !((GetKeyState(VK_XBUTTON2) & 0x100) != 0))
		{
			// Movement prediction
			float dx = targetEnemy.x + targetEnemy.velocityX*4 - x;
			float dy = targetEnemy.y + targetEnemy.velocityY*4 - y;

			Vector2* vec = window.GetWindowPosition();

			// Screen is flipped for team 2
			if (playerTeam == 2)
			{
				dx *= -1;
				dy *= -1;
			}

			// change this 69 till your cursor hits exactly on champ
			vec->x = 1920 / 2 + dx * 69;
			vec->y = 1080 / 2 - dy * 69;


			mouse.executeMovementTo(window, *vec);
			Sleep(50);
		}
	}

	return EXIT_SUCCESS;
}
