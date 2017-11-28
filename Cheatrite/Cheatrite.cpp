#include "Cheatrite.h"

Cheatrite::Cheatrite(string champ)
{
	this->champion = champ;
}

Cheatrite::~Cheatrite()
{
}

void Cheatrite::run()
{
	bool pageUpPressed = false;
	bool pageDownPressed = false;
	int offset = 50;
	bool controlPressed = false;
	bool enableAimbot = true;
	bool enableScripts = true;
	bool capsPressed = false;

	// EXPERIMENTAL
	bool cameraLocked = true;
	bool mButtonPressed = false;

	// Manually editing multiplier?
	float multiplier = 1.0f;

	MemoryManager memory;
	WindowManager window;
	MouseManager mouse;

	cout << "Searching for window.." << endl;
	while (!window.WindowFound())
	{
		window.FindWindow();
	}

	cout << "Window found!" << endl << endl;

	PlayerInformation playerInformation[20];


	while (window.WindowExists())
	{
		while (!window.WindowFocused())
		{
			Sleep(100);
		}

		if ((GetKeyState(VK_MBUTTON) & 0x8000) != 0)
		{
			if (mButtonPressed)
			{
				if (cameraLocked)
				{
					offset = offset + 34;
					cameraLocked = false;
				}
				else
				{
					offset = offset - 34;
					cameraLocked = true;
				}
			}
		}
		else
		{
			mButtonPressed = false;
		}

		if ((GetKeyState(VK_CAPITAL) & 0x8000) != 0)
		{
			if (!capsPressed)
			{
				capsPressed = true;
				enableScripts = !enableScripts;
			}
		}
		else
		{
			capsPressed = false;
		}

		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			if (!controlPressed)
			{
				controlPressed = true;
				enableAimbot = !enableAimbot;
			}
		}
		else
		{
			controlPressed = false;
		}

		if ((GetKeyState(VK_NEXT) & 0x8000) != 0)
		{
			if (!pageDownPressed)
			{
				pageDownPressed = true;
				offset--;
			}
		}
		else {
			pageDownPressed = false;
		}

		if ((GetKeyState(VK_PRIOR) & 0x8000) != 0)
		{
			if (!pageUpPressed)
			{
				pageUpPressed = true;
				offset++;
			}
		}
		else
		{
			pageUpPressed = false;
		}

		PlayerInformation localPlayer;

		// Get local players coordinates
		DWORD a1 = memory.Read<DWORD>(memory.Battlerite_Base + OFFSET_LOCAL_PLAYER[0]);
		DWORD a2 = memory.Read<DWORD>(a1 + OFFSET_LOCAL_PLAYER[1]);
		DWORD a3 = memory.Read<DWORD>(a2 + OFFSET_LOCAL_PLAYER[2]);
		DWORD a4 = memory.Read<DWORD>(a3 + OFFSET_LOCAL_PLAYER[3]);
		DWORD a5 = memory.Read<DWORD>(a4 + OFFSET_LOCAL_PLAYER[4]);

		localPlayer.x = memory.Read<float>(a5 + OFFSET_LOCAL_X);
		localPlayer.y = memory.Read<float>(a5 + OFFSET_LOCAL_Y);

		// Get champion list
		DWORD b1 = memory.Read<DWORD>(memory.MonoDll_Base + OFFSET_CHAMPION_LIST[0]);
		DWORD b2 = memory.Read<DWORD>(b1 + OFFSET_CHAMPION_LIST[1]);
		DWORD b3 = memory.Read<DWORD>(b2 + OFFSET_CHAMPION_LIST[2]);
		DWORD b4 = memory.Read<DWORD>(b3 + OFFSET_CHAMPION_LIST[3]);
		DWORD b5 = memory.Read<DWORD>(b4 + OFFSET_CHAMPION_LIST[4]);

		// Get Entity list
		DWORD c1 = memory.Read<DWORD>(memory.MonoDll_Base + OFFSET_ENTITY_LIST[0]);
		DWORD c2 = memory.Read<DWORD>(c1 + OFFSET_ENTITY_LIST[1]);
		DWORD c3 = memory.Read<DWORD>(c2 + OFFSET_ENTITY_LIST[2]);
		DWORD c4 = memory.Read<DWORD>(c3 + OFFSET_ENTITY_LIST[3]);

		// Find closest player for allies and enemies
		float closest1 = 1000000000.f;
		float closest2 = 1000000000.f;
		int closest1Index = -1;
		int closest2Index = -1;

		// Is a projectile going to hit us from team X
		bool projectileCollidesFromTeam1 = false;
		bool projectileCollidesFromTeam2 = false;

		// Local players team
		localPlayer.team = -1;

		// Loop through champions
		for (int i = 0; i < 10; i++)
		{
			ChampionInformation champion = memory.Read<ChampionInformation>(b5 + OFFSET_CHAMPION_START + i * CHAMPION_SIZE);

			// Ignore other teams
			if (champion.team != TEAM_1 && champion.team != TEAM_2)
				continue;

			// Out of map
			if (champion.x > 100.f || champion.x < -100.f || champion.y > 100.f || champion.y < -100.f)
				continue;

			// Ignore orb and null
			if (!champion.x || !champion.y)
				continue;

			// Distance to target
			float dx = localPlayer.x - champion.x;
			float dy = localPlayer.y - champion.y;
			float distanceToTarget = dx * dx + dy * dy;

			playerInformation[i].velocityX = champion.x - playerInformation[i].x;
			playerInformation[i].velocityY = champion.y - playerInformation[i].y;

			// Ignore dead people
			if (champion.currentHP <= 0.f)
				continue;

			// Ignore entities that are really far away
			if (distanceToTarget > (cameraLocked ? 200.f : 254.f))
				continue;

			if (distanceToTarget < 1.f)
			{
				// Local player found
				localPlayer.team = champion.team;
				localPlayer.x = champion.x;
				localPlayer.y = champion.y;
			}
			else if (champion.team == TEAM_1 && distanceToTarget < closest1)
			{
				closest1 = distanceToTarget;
				closest1Index = i;
			}
			else if (champion.team == TEAM_2 && distanceToTarget < closest2)
			{
				closest2 = distanceToTarget;
				closest2Index = i;
			}

			playerInformation[i].team = playerInformation[i].team;
			playerInformation[i].previousX = playerInformation[i].x;
			playerInformation[i].previousY = playerInformation[i].y;
			playerInformation[i].x = champion.x;
			playerInformation[i].y = champion.y;
			playerInformation[i].currentHP = champion.currentHP;
			playerInformation[i].maxHP = champion.maxHP;
			playerInformation[i].currentEnergy = champion.currentEnergy;
			playerInformation[i].maxEnergy = champion.maxEnergy;
		}


		// Loop through entities to find projectiles
		for (int i = 0; i < 15; i++)
		{
			EntityInformation entity = memory.Read<EntityInformation>(c4 + OFFSET_ENTITY_START + i * ENTITY_SIZE);

			// Ignore other teams
			if (entity.team != TEAM_1 && entity.team != TEAM_2)
				continue;

			// Out of map
			if (entity.x > 100.f || entity.x < -100.f || entity.y > 100.f || entity.y < -100.f)
				continue;

			// Ignore orb and null
			if (!entity.x || !entity.y)
				continue;

			// If moving projectile
			if (entity.directionX || entity.directionY)
			{
				// Trace ray with fixed range for all projectiles
				for (int i = 5; i < 200; i++)
				{
					float projectedX = entity.x + entity.directionX / 10 * i;
					float projectedY = entity.y + entity.directionY / 10 * i;

					float diffX = abs(projectedX - localPlayer.x);
					float diffY = abs(projectedY - localPlayer.y);

					// If within threshold
					if (diffX < 1.f && diffY < 1.f)
					{
						// We are going to / already have collided
						projectileCollidesFromTeam1 |= entity.team == TEAM_1;
						projectileCollidesFromTeam2 |= entity.team == TEAM_2;
						break;
					}
				}
			}
		}

		// Pick which closest player you want to target (ally or enemy)
		float distanceToEnemy = -1.f;
		float distanceToAlly = -1.f;
		PlayerInformation targetEnemy;
		PlayerInformation targetAlly;
		bool projectileWillHitUs = false;

		if (localPlayer.team == TEAM_2)
		{
			projectileWillHitUs = projectileCollidesFromTeam1;
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
		else if (localPlayer.team == TEAM_1)
		{
			projectileWillHitUs = projectileCollidesFromTeam2;
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

		static clock_t lastPressTime = clock();
		float differenceInTime = (clock() - lastPressTime) / CLOCKS_PER_SEC;

		// Todo: Add cooldowns back, I just commented it out from pearl, and it sucks :(

		if (enableScripts)
		{
			if (this->champion == "Jade")
			{
				// Jade scripts
				if (distanceToEnemy > 0.f && differenceInTime > 0.5)
				{
					INPUT keyEvent;
					keyEvent.type = INPUT_KEYBOARD;
					keyEvent.ki.wScan = 0;
					keyEvent.ki.time = 0;
					keyEvent.ki.dwExtraInfo = 0;

					if (distanceToEnemy < 5.f)
					{
						// If very close then jump

						lastPressTime = clock();

						// Press the space key
						keyEvent.ki.wVk = VK_SPACE; // virtual-key code for the space key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the space key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					if (distanceToEnemy < 10.f)
					{
						// Auto Knockback if very close, change to 20.f for normal range

						lastPressTime = clock();

						// Press the "R" key
						keyEvent.ki.wVk = 0x52; // virtual-key code for the "r" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the "R" key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					if ((distanceToEnemy < 30.f && distanceToAlly < 20.f) || projectileWillHitUs)
					{
						// Auto EX STEALTH if near

						// if in range cast 2
						lastPressTime = clock();

						// Press the "2" key
						keyEvent.ki.wVk = 0x32; // virtual-key code for the "2" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the "2" key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					else if (distanceToEnemy > 20.f && distanceToEnemy < 100.f)
					{
					  // Auto EX SNIPE if not close and in range

					  // if in range cast 1
					  lastPressTime = clock();

					  // Press the "1" key
					  keyEvent.ki.wVk = 0x31; // virtual-key code for the "1" key
					  keyEvent.ki.dwFlags = 0; // 0 for key press
					  SendInput(1, &keyEvent, sizeof(INPUT));

					  // Release the "1" key
					  keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
					  SendInput(1, &keyEvent, sizeof(INPUT));
					}
				}
			}
			else if (this->champion == "Ashka")
			{
				if (distanceToEnemy > 0.f && differenceInTime > 0.5)
				{
					INPUT keyEvent;
					keyEvent.type = INPUT_KEYBOARD;
					keyEvent.ki.wScan = 0;
					keyEvent.ki.time = 0;
					keyEvent.ki.dwExtraInfo = 0;

					if (distanceToEnemy < 5.f)
					{
						// If very close then petrify (EX Flame strike)

						lastPressTime = clock();

						// Press the '2' key
						keyEvent.ki.wVk = 0x32; // virtual-key code for the '2' key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the space key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					if (distanceToEnemy < 5.f)
					{
						// Auto Knockback if close
						lastPressTime = clock();

						// Press the "E" key
						keyEvent.ki.wVk = 0x45; // virtual-key code for the "E" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the "E" key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					//if (distanceToEnemy < 30.f)
					//{
					//  // if in range cast R
					//  lastPressTime = clock();

					//  // Press the "R" key
					//  keyEvent.ki.wVk = 0x52; // virtual-key code for the "R" key
					//  keyEvent.ki.dwFlags = 0; // 0 for key press
					//  SendInput(1, &keyEvent, sizeof(INPUT));

					//  // Release the "R" key
					//  keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
					//  SendInput(1, &keyEvent, sizeof(INPUT));
					//}
					if (distanceToEnemy > 20.f && distanceToEnemy < 100.f)
					{
						// if in range cast Q
						lastPressTime = clock();

						// Press the "Q" key
						keyEvent.ki.wVk = 0x51; // virtual-key code for the "Q" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the "Q" key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
				}
			}
			else if (this->champion == "Pearl")
			{
				if (distanceToEnemy > 0.f)
				{
					INPUT keyEvent;
					keyEvent.type = INPUT_KEYBOARD;
					keyEvent.ki.wScan = 0;
					keyEvent.ki.time = 0;
					keyEvent.ki.dwExtraInfo = 0;

					if (/*!cooldownSPACE && */distanceToEnemy < 5.f)
					{
						// If very close then jump

						// Press the space key
						keyEvent.ki.wVk = VK_SPACE; // virtual-key code for the space key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the space key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Move opposite direction of enemy
						multiplier = -100.f;
					}
					else if (/*!cooldownQ && */projectileWillHitUs && distanceToEnemy > 30.f)
					{
						// Auto Counter

						keyEvent.ki.wVk = 0x51; // virtual-key code for the "Q" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					else if (/*!cooldownE && */projectileWillHitUs)
					{
						// Auto Bubble

						keyEvent.ki.wVk = 0x45; // virtual-key code for the "E" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));

						multiplier = 0.5f;
					}
					else if (/*!cooldownR && */projectileWillHitUs)
					{
						// Auto Shield

						keyEvent.ki.wVk = 0x52; // virtual-key code for the "R" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					//else if (distanceToEnemy > 100.f)
					//{
					//  keyEvent.ki.wVk = 0x31; // virtual-key code for the "1" key
					//  keyEvent.ki.dwFlags = 0; // 0 for key press
					//  SendInput(1, &keyEvent, sizeof(INPUT));

					//  keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
					//  SendInput(1, &keyEvent, sizeof(INPUT));
					//}
				}
			}
			else if (this->champion == "Ezmo")
			{
				if (distanceToEnemy > 0.f && differenceInTime > 0.5)
				{
					INPUT keyEvent;
					keyEvent.type = INPUT_KEYBOARD;
					keyEvent.ki.wScan = 0;
					keyEvent.ki.time = 0;
					keyEvent.ki.dwExtraInfo = 0;

					if (projectileWillHitUs && distanceToEnemy > 30.f)
					{
						// Auto block projectile

						lastPressTime = clock();

						// Press the "Q" key
						keyEvent.ki.wVk = 0x51; // virtual-key code for the "Q" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the space key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					if (distanceToEnemy < 60.f && distanceToEnemy > 20.f)
					{
						// Auto Root
						lastPressTime = clock();

						// Press the "E" key
						keyEvent.ki.wVk = 0x45; // virtual-key code for the "E" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the "E" key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					if (distanceToEnemy < 40.f)
					{
						// Auto totem
						lastPressTime = clock();

						// Press the "R" key
						keyEvent.ki.wVk = 0x52; // virtual-key code for the "R" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the "R" key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
				}
			}
			else if (this->champion == "Raigon")
			{
				if (distanceToEnemy > 0.f && differenceInTime > 0.5)
				{
					INPUT keyEvent;
					keyEvent.type = INPUT_KEYBOARD;
					keyEvent.ki.wScan = 0;
					keyEvent.ki.time = 0;
					keyEvent.ki.dwExtraInfo = 0;

					if (projectileWillHitUs && distanceToEnemy < 80.f)
					{
						// Auto block projectile
						lastPressTime = clock();

						// Press the "Q" key
						keyEvent.ki.wVk = 0x51; // virtual-key code for the "Q" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the space key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					if (distanceToEnemy < 100.f && distanceToEnemy > 40.f)
					{
						// Auto Ex Sesimic Shock
						lastPressTime = clock();

						// Press the "2" key
						keyEvent.ki.wVk = 0x32; // virtual-key code for the "2" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the "2" key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
					if (distanceToEnemy < 100.f && distanceToEnemy > 40.f)
					{
						// Auto Sesimic Shock
						lastPressTime = clock();

						// Press the "E" key
						keyEvent.ki.wVk = 0x45; // virtual-key code for the "E" key
						keyEvent.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &keyEvent, sizeof(INPUT));

						// Release the "E" key
						keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &keyEvent, sizeof(INPUT));
					}
				}
			}
		}

		if (enableAimbot)
		{
			// The aimbot
			// If mouse button 5 is not pressed then aim at closest target
			if (distanceToEnemy > 1.f)
			{
				// Movement prediction
				float dx = targetEnemy.x + targetEnemy.velocityX * 5 - localPlayer.x;
				float dy = targetEnemy.y + targetEnemy.velocityY * 5 - localPlayer.y;

				Vector2 vec = window.GetWindowPosition();

				// Screen is flipped for team 2
				if (localPlayer.team == TEAM_2)
				{
					dx = -1;
					dy = -1;
				}

				// change this 69 till your cursor hits exactly on champ
				vec.x = GetSystemMetrics(SM_CXSCREEN) / 2 + multiplier * (dx * offset);
				vec.y = GetSystemMetrics(SM_CYSCREEN) / 2 - multiplier * (dy * offset);

				// Auto heal on right click
				if (enableScripts && this->champion == "Pearl" && (GetKeyState(VK_RBUTTON) & 0x100) != 0)
				{
					vec.x = GetSystemMetrics(SM_CXSCREEN) / 2;
					vec.y = GetSystemMetrics(SM_CYSCREEN) / 2;
				}

				mouse.executeMovementTo(window, vec);
				Sleep(50);
			}
		}
	}
}