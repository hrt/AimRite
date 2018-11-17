#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "WindowManager.hpp"
#include "MouseManager.hpp"
#include "MemoryManager.h"
#include "Offsets.hpp"
#include "Records.hpp"
#include <windows.h>
#include <tlhelp32.h>


BOOL CompareBytes(byte* data, byte* mask, char *_mask)
{
	for (; *_mask; ++_mask, ++data, ++mask)
		if (*_mask == 'x' && *data != *mask)
			return 0;

	return (*_mask == 0);
}

uintptr_t FindPattern(HANDLE processHandle, uintptr_t start, size_t size, char* sig, char* mask)
{
	byte* data = new byte[size];

	if (!ReadProcessMemory(processHandle, (void*)start, data, size, 0))
		return 0;

	for (uintptr_t i = 0; i < size; i++)
		if (CompareBytes((byte*)(data + i), (byte*)sig, mask))
			return start + i;
	delete[] data;
	return 0;
}



template<class c>
c Read(HANDLE processHandle, DWORD dwAddress)
{
	c val;
	ReadProcessMemory(processHandle, (LPVOID)dwAddress, &val, sizeof(c), NULL);
	return val;
}

template<class c>
BOOL Write(HANDLE processHandle, DWORD dwAddress, c ValueToWrite)
{
	return WriteProcessMemory(processHandle, (LPVOID)dwAddress, &ValueToWrite, sizeof(c), NULL);
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
	std::cout << "[~] Window found!" << std::endl << std::endl;

	std::cout << std::hex << "[~] battlerite.exe base  : " << memory.Battlerite_Base << std::endl;
	std::cout << std::hex << "[~] mono.dll base  : " << (DWORD)memory.MonoDll_Base << std::endl;
	DWORD p1 = Read<DWORD>(memory.handle, memory.Battlerite_Base + OFFSET_LOCAL_PLAYER[0]);
	DWORD p2 = Read<DWORD>(memory.handle, p1 + OFFSET_LOCAL_PLAYER[1]);

	float x = Read<float>(memory.handle, p2 + OFFSET_LOCAL_X);
	std::cout << "[~] Player x coordinate : " << x << std::endl;




	std::vector<MEMORY_BASIC_INFORMATION> memInfos;
	std::vector<MEMORY_BASIC_INFORMATION> readableMemInfos;
	MEMORY_BASIC_INFORMATION memInfo;

	unsigned char* addr;
	for (addr = 0; VirtualQueryEx(memory.handle, addr, &memInfo, sizeof(memInfo)) == sizeof(memInfo); addr += memInfo.RegionSize)
	{
		memInfos.push_back(memInfo);
	}

	for (int i = 0; i < memInfos.size(); i++)
	{
		DWORD prot = memInfos[i].Protect;
		if (prot == PAGE_EXECUTE_READ || prot == PAGE_EXECUTE_READWRITE || prot == PAGE_READONLY || prot == prot == PAGE_READWRITE)
			readableMemInfos.push_back(memInfos[i]);
	}

	std::vector<DWORD> championListPointers;
	std::vector<DWORD> cooldownListPointers;
	for (int i = 0; i < readableMemInfos.size(); i++)
	{
		if ((uint64_t)readableMemInfos[i].BaseAddress < 0xffffffff)
		{
			DWORD _championListPointer = (DWORD)FindPattern(memory.handle, (DWORD)readableMemInfos[i].BaseAddress, readableMemInfos[i].RegionSize, "\x9A\xC1\x00\x00\xC0\xBF\x5A\x20\x33\x32\x01\x01", "xxxxxxxxxxxx");
			if (_championListPointer)
			{
				_championListPointer -= 26;
				championListPointers.push_back(_championListPointer);
			}

			DWORD _cooldownListPointer = (DWORD)FindPattern(memory.handle, (DWORD)readableMemInfos[i].BaseAddress, readableMemInfos[i].RegionSize, "\xB5\x1B\xB6\x31\x01\x01", "xxxxxx");
			if (_cooldownListPointer)
			{
				_cooldownListPointer -= 16;
				cooldownListPointers.push_back(_cooldownListPointer);
			}
		}
	}

	std::cout << "[~] Found " << championListPointers.size() << " champion list pointers!" << std::endl;
	std::cout << "[~] Found " << cooldownListPointers.size() << " cooldown list pointers!" << std::endl;

	if (championListPointers.size() == 0 || cooldownListPointers.size() == 0)
	{
		std::cout << "[!] Please select Iva and go to the playground before running this" << std::endl;
	}

	DWORD championlistPointer = championListPointers[0];
	DWORD cooldownListPointer = cooldownListPointers[0];

	x = Read<float>(memory.handle, championlistPointer + OFFSET_CHAMPION_X);
	std::cout << "[~] Entity x coordinate : " << x << std::endl;


	Records* records[10];
	for (int i = 0; i < 10; i++)
	{
		records[i] = new Records(35);
	}


	while (window.WindowFocused() || window.WindowExists())
	{
		// zoom
		//Write<float>(memory.handle, 0x3DD087D8, 40.f);

		//// Get local players buttons
		//DWORD b1 = Read<DWORD>(memory.handle, memory.Battlerite_Base + OFFSET_LOCAL_BUTTONS[0]);
		//DWORD b2 = Read<DWORD>(memory.handle, b1 + OFFSET_LOCAL_BUTTONS[1]);
		//DWORD b3 = Read<DWORD>(memory.handle, b2 + OFFSET_LOCAL_BUTTONS[2]);
		//DWORD b4 = Read<DWORD>(memory.handle, b3 + OFFSET_LOCAL_BUTTONS[3]);
		//DWORD b5 = Read<DWORD>(memory.handle, b4 + OFFSET_LOCAL_BUTTONS[4]);

		////Write<int>(memory.handle, b5 + OFFSET_LOCAL_ALPHA, MOVE_LEFT + MOVE_DOWN);


		float cooldownQ = Read<float>(memory.handle, cooldownListPointer + OFFSET_COOLDOWNS_Q);
		float cooldownR = Read<float>(memory.handle, cooldownListPointer + OFFSET_COOLDOWNS_R);
		float cooldownE = Read<float>(memory.handle, cooldownListPointer + OFFSET_COOLDOWNS_E);
		float cooldownRIGHT = Read<float>(memory.handle, cooldownListPointer + OFFSET_COOLDOWNS_RIGHT);
		float cooldownSPACE = Read<float>(memory.handle, cooldownListPointer + OFFSET_COOLDOWNS_SPACE);

		//std::cout << cooldownQ << std::endl;
		//std::cout << cooldownR << std::endl;
		//std::cout << cooldownE << std::endl;
		//std::cout << cooldownRIGHT << std::endl;
		//std::cout << cooldownSPACE << std::endl; 

		// Get local players coordinates
		DWORD p1 = Read<DWORD>(memory.handle, memory.Battlerite_Base + OFFSET_LOCAL_PLAYER[0]);
		DWORD p2 = Read<DWORD>(memory.handle, p1 + OFFSET_LOCAL_PLAYER[1]);

		float x = Read<float>(memory.handle, p2 + OFFSET_LOCAL_X);
		float y = Read<float>(memory.handle, p2 + OFFSET_LOCAL_Y);

		// Find closest player for allies and enemies
		float closest1 = 1000000000.f;
		float closest2 = 1000000000.f;
		float lowest1 = 1000000000.f;
		float lowest2 = 1000000000.f;
		float distanceToLowest1 = 1000000000.f;
		float distanceToLowest2 = 1000000000.f;
		int closest1Index = -1;
		int closest2Index = -1;
		int lowest1Index = -1;
		int lowest2Index = -1;

		// Is a projectile going to hit us from team X
		bool projectileCollidesFromTeam1 = false;
		bool projectileCollidesFromTeam2 = false;
		static clock_t abilityPressedTimeStamp = clock();
		float timeSinceCast = (clock() - abilityPressedTimeStamp) * 1000 / CLOCKS_PER_SEC;
		static float abilityMultiplier = 1.f;

		// Local players team
		int playerTeam = -1;
		float playerEnergy = 0;
		float playerHealth = 0;
		bool playerCasting = false;
		float playerMaxHealth = 0;
		DWORD playerStatus = 0;
		int playerAmmo = 0;
		// Loop through entities
		for (int i = 0; i < 10; i++)
		{
			float targetX = Read<float>(memory.handle, championlistPointer + OFFSET_CHAMPION_X + i * PLAYER_SIZE);
			float targetY = Read<float>(memory.handle, championlistPointer + OFFSET_CHAMPION_Y + i * PLAYER_SIZE);
			float targetHealth = Read<float>(memory.handle, championlistPointer + OFFSET_CHAMPION_HEALTH + i * PLAYER_SIZE);
			float targetMaxHealth = Read<float>(memory.handle, championlistPointer + OFFSET_CHAMPION_MAX_HEALTH + i * PLAYER_SIZE);
			float targetCasting = Read<float>(memory.handle, championlistPointer + OFFSET_CHAMPION_CASTING + i * PLAYER_SIZE);
			int targetTeam = Read<int>(memory.handle, championlistPointer + OFFSET_CHAMPION_TEAM + i * PLAYER_SIZE);
			DWORD targetStatus = Read<DWORD>(memory.handle, championlistPointer + OFFSET_CHAMPION_STATUS + i * PLAYER_SIZE);


			// Ignore weak entities like orb, blossom tree and illusions
			if (targetMaxHealth < 190.f)
			{
				continue;
			}

			// Ignore neutral teams
			if (targetTeam != 1 && targetTeam != 2)
				continue;

			// ignore non alive entities
			if (targetHealth <= 0)
				continue;

			// Out of map
			if (targetX > 100.f || targetX < -100.f || targetY > 100.f || targetY < -100.f)
				continue;

			//// Ignore orb and null
			//if (!targetX || !targetY)
			//	continue;


			PlayerInformation* latestRecord = records[i]->add(targetX, targetY);
			PlayerInformation* oldestRecord = records[i]->getOldest();

			latestRecord->velocityX = (oldestRecord->x - latestRecord->x) * 4;
			latestRecord->velocityY = (oldestRecord->y - latestRecord->y) * 4;

			// Ignore dead people or afk people (1 seconds)
			if (targetStatus != 0 || abs(latestRecord->velocityX) > 0.1f
				|| abs(latestRecord->velocityY) > 0.1f)
			{
				// Update timer
				latestRecord->lastUpdate = clock();
			}

			// Distance to target
			float dx = x - targetX + latestRecord->velocityX;
			float dy = y - targetY + latestRecord->velocityY;
			float distanceToTarget = dx * dx + dy * dy;


			float differenceInTime = (clock() - latestRecord->lastUpdate) / CLOCKS_PER_SEC;

			if (differenceInTime > 1.f)
				continue;

			// Ignore entities that are really far away
			if (distanceToTarget > 1000.f)
				continue;

			latestRecord->previousX = oldestRecord->x;
			latestRecord->previousY = oldestRecord->y;
			latestRecord->x = targetX;
			latestRecord->y = targetY;
			latestRecord->invulnerable = std::find(std::begin(DEFENCE_ABILITY), std::end(DEFENCE_ABILITY), targetStatus) != std::end(DEFENCE_ABILITY);
			latestRecord->castingImportant = std::find(std::begin(IMPORTANT_CAST), std::end(IMPORTANT_CAST), targetStatus) != std::end(IMPORTANT_CAST);
			latestRecord->casting = targetCasting > 0.f && targetCasting < 0.85f;


			if (distanceToTarget < 1.f)
			{
				// Local player found
				playerTeam = targetTeam;
				playerStatus = targetStatus;
				playerEnergy = Read<float>(memory.handle, championlistPointer + OFFSET_CHAMPION_ENERGY + i * PLAYER_SIZE);
				playerHealth = targetHealth;
				playerMaxHealth = Read<float>(memory.handle, championlistPointer + OFFSET_CHAMPION_MAX_HEALTH + i * PLAYER_SIZE);
				playerAmmo = Read<int>(memory.handle, championlistPointer + OFFSET_CHAMPION_BULLETS_LEFT + i * PLAYER_SIZE);
				playerCasting = targetCasting > 0.f && targetCasting < 0.85f;
			}
			else if (targetTeam == 1)
			{
				if (distanceToTarget < closest1)
				{
					closest1 = distanceToTarget;
					closest1Index = i;
				}


				if (distanceToTarget < AUTO_ATTACK_RANGE && (targetHealth < lowest1 || latestRecord->casting) && (!latestRecord->invulnerable || lowest1Index == -1))
				{
					lowest1 = targetHealth;
					lowest1Index = i;
					distanceToLowest1 = distanceToTarget;
				}
			}
			else if (targetTeam == 2)
			{
				if (distanceToTarget < closest2)
				{
					closest2 = distanceToTarget;
					closest2Index = i;
				}

				if (distanceToTarget < AUTO_ATTACK_RANGE && (targetHealth < lowest2 || latestRecord->casting) && (!latestRecord->invulnerable || lowest2Index == -1))
				{
					lowest2 = targetHealth;
					lowest2Index = i;
					distanceToLowest2 = distanceToTarget;
				}
			}
		}
		// Pick which closest player you want to target (ally or enemy)
		float distanceToEnemy = -1.f;
		float distanceToClosestEnemy = -1.f;
		float distanceToAlly = -1.f;
		PlayerInformation* targetEnemy = NULL;
		PlayerInformation* targetAlly = NULL;
		bool projectileWillHitUs = false;

		//std::cout << cooldownE << std::endl;

		if (playerTeam == 2)
		{
			projectileWillHitUs = projectileCollidesFromTeam1;
			distanceToClosestEnemy = closest1;
			if (lowest1Index != -1)
			{
				distanceToEnemy = distanceToLowest1;
				targetEnemy = records[lowest1Index]->getLatest();
			}
			else if (closest1Index != -1)
			{
				distanceToEnemy = closest1;
				targetEnemy = records[closest1Index]->getLatest();
			}

			if (closest2Index != -1)
			{
				distanceToAlly = closest2;
				targetAlly = records[closest2Index]->getLatest();
			}
		}
		else if (playerTeam == 1)
		{
			projectileWillHitUs = projectileCollidesFromTeam2;
			distanceToClosestEnemy = closest2;
			if (closest1Index != -1)
			{
				distanceToAlly = closest1;
				targetAlly = records[closest1Index]->getLatest();
			}

			if (lowest2Index != -1)
			{
				distanceToEnemy = distanceToLowest2;
				targetEnemy = records[lowest2Index]->getLatest();
			}
			else if (closest2Index != -1)
			{
				distanceToEnemy = closest2;
				targetEnemy = records[closest2Index]->getLatest();
			}
		}
		else
		{
			// Local player is not on a team, may not be in game or dead so do not move mouse
			continue;
		}
		//std::cout << targetEnemy->x << std::endl;

		// Do not case aggressive spells if mouse button 5 is held
		bool passivePlay = (GetKeyState(VK_XBUTTON2) & 0x100) != 0;
		// The aimbot
		// If mouse button 5 is not pressed then aim at closest target
		if (timeSinceCast > 0 && timeSinceCast < 400 && targetEnemy)
		{
			float dx = targetEnemy->x - x;
			float dy = targetEnemy->y - y;
			dx *= abilityMultiplier;
			dy *= abilityMultiplier;

			// Screen is flipped for team 2
			if (playerTeam == 2)
			{
				dx *= -1;
				dy *= -1;
			}
			Vector2* vec = window.GetWindowPosition();

			vec->x = SCREEN_WIDTH / 2 + dx * SCALE;
			vec->y = SCREEN_HEIGHT / 2 - dy * SCALE;


			mouse.executeMovementTo(window, *vec);
			Sleep(1);

		}
		else if (distanceToEnemy > 1.f && !passivePlay && targetEnemy)
		{
			// Movement prediction
			float dx = targetEnemy->x - x;
			float dy = targetEnemy->y - y;


			Vector2* vec = window.GetWindowPosition();

			INPUT keyEvent;
			keyEvent.type = INPUT_KEYBOARD;
			keyEvent.ki.wScan = 0;
			keyEvent.ki.time = 0;
			keyEvent.ki.dwExtraInfo = 0;

			// Screen is flipped for team 2
			if ((playerTeam == 2 && !targetEnemy->invulnerable/* && playerAmmo > 0*/) || (playerTeam == 1 && (targetEnemy->invulnerable/* || playerAmmo == 0*/)))
			{
				dx *= -1;
				dy *= -1;
			}


			// change this 71 till your cursor hits exactly on champ
			vec->x = SCREEN_WIDTH / 2 + dx * SCALE;
			vec->y = SCREEN_HEIGHT / 2 - dy * SCALE;


			mouse.executeMovementTo(window, *vec);

			if (playerStatus == IVA_IN_ULT)
			{
				Sleep(1);
				continue;
			}

			if ((targetEnemy->invulnerable || distanceToEnemy > 250.f) && playerCasting)
			{
				// CANCEL whatever we are casting
				keyEvent.ki.wVk = 0x43; // virtual-key code for the c key
				keyEvent.ki.dwFlags = 0; // 0 for key press
				SendInput(1, &keyEvent, sizeof(INPUT));

				// Release the c key
				keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
				SendInput(1, &keyEvent, sizeof(INPUT));
			}
			// auto shield
			else if (distanceToClosestEnemy < 15.f && (targetEnemy->casting || playerAmmo <= 0) && cooldownQ == 0)
			{
				// Press the q key
				keyEvent.ki.wVk = 0x51; // virtual-key code for the q key
				keyEvent.ki.dwFlags = 0; // 0 for key press
				SendInput(1, &keyEvent, sizeof(INPUT));

				// Release the q key
				keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
				SendInput(1, &keyEvent, sizeof(INPUT));

				abilityPressedTimeStamp = clock();
				abilityMultiplier = 0.f;
			}
			// auto E if target is using important ability
			else if (((targetEnemy->casting /*&& playerAmmo <= 0*/) || targetEnemy->castingImportant) && distanceToEnemy < 125.f && cooldownE == 0 && !playerCasting)
			{
				// Press the "E" key
				keyEvent.ki.wVk = 0x45; // virtual-key code for the "E" key
				if (playerEnergy > 25.f && distanceToClosestEnemy != distanceToEnemy)
				{
					keyEvent.ki.wVk = 0x32; // virtual-key code for the "2" key

				}
				keyEvent.ki.dwFlags = 0; // 0 for key press
				SendInput(1, &keyEvent, sizeof(INPUT));

				// Release the "E" key
				keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
				SendInput(1, &keyEvent, sizeof(INPUT));
			}
			// If very close then jump
			//else if (distanceToClosestEnemy < 7.f && cooldownSPACE == 0 && playerAmmo <= 0 && playerStatus != IVA_CAST_ULT && playerStatus != IVA_IN_ULT)
			//{
			//	// Press the space key
			//	keyEvent.ki.wVk = VK_SPACE; // virtual-key code for the space key
			//	keyEvent.ki.dwFlags = 0; // 0 for key press
			//	SendInput(1, &keyEvent, sizeof(INPUT));

			//	// Release the space key
			//	keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
			//	SendInput(1, &keyEvent, sizeof(INPUT));

			//	abilityPressedTimeStamp = clock();
			//	abilityMultiplier = 2.f;
			//}
			// Auto Right if not close and in range
			else if (distanceToEnemy > 20.f && distanceToEnemy < 125.f && cooldownRIGHT == 0 && !playerCasting)
			{
				// Press the "3" key
				keyEvent.ki.wVk = 0x33; // virtual-key code for the "3" key
				keyEvent.ki.dwFlags = 0; // 0 for key press
				SendInput(1, &keyEvent, sizeof(INPUT));

				// Release the "3" key
				keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
				SendInput(1, &keyEvent, sizeof(INPUT));
			}
			else if (playerAmmo > 0 && distanceToEnemy < 60.f && !playerCasting)
			{
				// Press the "4" key
				keyEvent.ki.wVk = 0x34; // virtual-key code for the "4" key
				keyEvent.ki.dwFlags = 0; // 0 for key press
				SendInput(1, &keyEvent, sizeof(INPUT));

				// Release the "4" key
				keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
				SendInput(1, &keyEvent, sizeof(INPUT));
			}
			else if (distanceToEnemy < 125.f && cooldownR == 0 && playerAmmo == 0 && targetEnemy->castingImportant && playerEnergy > 25.f && !playerCasting)
			{
				// Press the "r" key
				keyEvent.ki.wVk = 0x52; // virtual-key code for the "r" key
				keyEvent.ki.dwFlags = 0; // 0 for key press
				SendInput(1, &keyEvent, sizeof(INPUT));

				// Release the "r" key
				keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
				SendInput(1, &keyEvent, sizeof(INPUT));
			}

			Sleep(1);
		}
	}

	return EXIT_SUCCESS;
}
