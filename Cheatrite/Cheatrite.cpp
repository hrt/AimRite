#include "Cheatrite.h"

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

Cheatrite::Cheatrite(string champ)
{
	this->champion = champ;
}

Cheatrite::~Cheatrite()
{
}

void Cheatrite::run()
{
	bool numAddPressed = false;
	bool numSubtractPressed = false;
	int offset = 50;
	bool controlPressed = false;
	bool enableAimbot = true;
	bool enableScripts = true;
	bool capsPressed = false;

	// EXPERIMENTAL
	bool cameraLocked = true;
	bool mButtonPressed = false;

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

	goto windowFocused;

windowFocused:
	{
		while (window.WindowExists() && window.WindowFocused()) {
			if ((GetKeyState(VK_MBUTTON) & 0x8000) != 0) {
				if (mButtonPressed) {
					if (cameraLocked) {
						offset = offset + 34;
						cameraLocked = false;
					}
					else {
						offset = offset - 34;
						cameraLocked = true;
					}
				}
			}
			else {
				mButtonPressed = false;
			}

			if ((GetKeyState(VK_CAPITAL) & 0x8000) != 0) {
				if (!capsPressed) {
					capsPressed = true;
					enableScripts = !enableScripts;
				}
			}
			else {
				capsPressed = false;
			}

			if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
				if (!controlPressed) {
					controlPressed = true;
					enableAimbot = !enableAimbot;
				}
			}
			else {
				controlPressed = false;
			}

			if ((GetKeyState(VK_SUBTRACT) & 0x8000) != 0) {
				if (!numSubtractPressed) {
					numSubtractPressed = true;
					offset--;
				}
			}
			else {
				numSubtractPressed = false;
			}

			if ((GetKeyState(VK_ADD) & 0x8000) != 0) {
				if (!numAddPressed) {
					numAddPressed = true;
					offset++;
				}
			}
			else {
				numAddPressed = false;
			}

			// Get local players buttons
			DWORD b1 = Read<DWORD>(memory.handle, memory.Battlerite_Base + OFFSET_LOCAL_BUTTONS[0]);
			DWORD b2 = Read<DWORD>(memory.handle, b1 + OFFSET_LOCAL_BUTTONS[1]);
			DWORD b3 = Read<DWORD>(memory.handle, b2 + OFFSET_LOCAL_BUTTONS[2]);
			DWORD b4 = Read<DWORD>(memory.handle, b3 + OFFSET_LOCAL_BUTTONS[3]);
			DWORD b5 = Read<DWORD>(memory.handle, b4 + OFFSET_LOCAL_BUTTONS[4]);

			//Write<int>(memory.handle, b5 + OFFSET_LOCAL_ALPHA, MOVE_LEFT + MOVE_DOWN);

			// Get local players coordinates
			DWORD c1 = Read<DWORD>(memory.handle, memory.Battlerite_Base + OFFSET_LOCAL_PLAYER[0]);
			DWORD c2 = Read<DWORD>(memory.handle, c1 + OFFSET_LOCAL_PLAYER[1]);
			DWORD c3 = Read<DWORD>(memory.handle, c2 + OFFSET_LOCAL_PLAYER[2]);
			DWORD c4 = Read<DWORD>(memory.handle, c3 + OFFSET_LOCAL_PLAYER[3]);
			DWORD c5 = Read<DWORD>(memory.handle, c4 + OFFSET_LOCAL_PLAYER[4]);

			float x = Read<float>(memory.handle, c5 + OFFSET_LOCAL_X);
			float y = Read<float>(memory.handle, c5 + OFFSET_LOCAL_Y);

			// Get entity list
			DWORD e1 = Read<DWORD>(memory.handle, memory.MonoDll_Base + OFFSET_ENTITY_LIST[0]);
			DWORD e2 = Read<DWORD>(memory.handle, e1 + OFFSET_ENTITY_LIST[1]);
			DWORD e3 = Read<DWORD>(memory.handle, e2 + OFFSET_ENTITY_LIST[2]);
			DWORD e4 = Read<DWORD>(memory.handle, e3 + OFFSET_ENTITY_LIST[3]);

			// Find closest player for allies and enemies
			float closest1 = 1000000000.f;
			float closest2 = 1000000000.f;
			int closest1Index = -1;
			int closest2Index = -1;

			// Is a projectile going to hit us from team X
			bool projectileCollidesFromTeam1 = false;
			bool projectileCollidesFromTeam2 = false;

			// Local players team
			int playerTeam = -1;

			// Loop through entities
			for (int i = 0; i < 10; i++)
			{
				float targetX = Read<float>(memory.handle, e4 + OFFSET_ENTITY_START + OFFSET_ENTITY_X + i * PLAYER_SIZE);
				int targetTeam = Read<int>(memory.handle, e4 + OFFSET_ENTITY_START + OFFSET_ENTITY_TEAM + i * PLAYER_SIZE);
				float targetY = Read<float>(memory.handle, e4 + OFFSET_ENTITY_START + OFFSET_ENTITY_Y + i * PLAYER_SIZE);

				// 1 is right -1 is left
				float targetDirectionX = Read<float>(memory.handle, e4 + OFFSET_ENTITY_START + OFFSET_ENTITY_DIRECTION_X + i * PLAYER_SIZE);
				// 1 is up -1 is down
				float targetDirectionY = Read<float>(memory.handle, e4 + OFFSET_ENTITY_START + OFFSET_ENTITY_DIRECTION_Y + i * PLAYER_SIZE);

				// Ignore other teams
				if (targetTeam != 1 && targetTeam != 2)
					continue;

				// Out of map
				if (targetX > 100.f || targetX < -100.f || targetY > 100.f || targetY < -100.f)
					continue;

				if (targetDirectionX || targetDirectionY)
				{
					// Trace ray with fixed range for all projectiles
					for (int i = 5; i < 200; i++)
					{
						float projectedX = targetX + targetDirectionX / 10 * i;
						float projectedY = targetY + targetDirectionY / 10 * i;

						float diffX = abs(projectedX - x);
						float diffY = abs(projectedY - y);

						// If within threshold
						if (diffX < 1.f && diffY < 1.f)
						{
							// We are going to / already have collided
							projectileCollidesFromTeam1 |= targetTeam == 1;
							projectileCollidesFromTeam2 |= targetTeam == 2;
							break;
						}
					}
				}

				// Don't aim at projectiles
				if (targetDirectionX || targetDirectionY)
					continue;

				// Ignore orb and null
				if (!targetX || !targetY)
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
			bool projectileWillHitUs = false;

			if (playerTeam == 2)
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
			else if (playerTeam == 1)
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

			if (enableScripts) {
				if (this->champion == "Jade") {
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
						//else if (distanceToEnemy > 20.f && distanceToEnemy < 100.f && !passivePlay)
						//{
						//	// Auto EX SNIPE if not close and in range

						//	// if in range cast 1
						//	lastPressTime = clock();

						//	// Press the "1" key
						//	keyEvent.ki.wVk = 0x31; // virtual-key code for the "1" key
						//	keyEvent.ki.dwFlags = 0; // 0 for key press
						//	SendInput(1, &keyEvent, sizeof(INPUT));

						//	// Release the "1" key
						//	keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						//	SendInput(1, &keyEvent, sizeof(INPUT));
						//}
					}
				}
				else if (this->champion == "Ashka") {
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
						//	// if in range cast R
						//	lastPressTime = clock();

						//	// Press the "R" key
						//	keyEvent.ki.wVk = 0x52; // virtual-key code for the "R" key
						//	keyEvent.ki.dwFlags = 0; // 0 for key press
						//	SendInput(1, &keyEvent, sizeof(INPUT));

						//	// Release the "R" key
						//	keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						//	SendInput(1, &keyEvent, sizeof(INPUT));
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
				else if (this->champion == "Pearl") {
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
							//multiplier = -100.f;
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

							//multiplier = 0.5f;
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
						//	keyEvent.ki.wVk = 0x31; // virtual-key code for the "1" key
						//	keyEvent.ki.dwFlags = 0; // 0 for key press
						//	SendInput(1, &keyEvent, sizeof(INPUT));

						//	keyEvent.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						//	SendInput(1, &keyEvent, sizeof(INPUT));
						//}
					}
				}
				else if (this->champion == "Ezmo") {
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
				else if (this->champion == "Ragion") {
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

			if (enableAimbot) {
				// The aimbot
				// If mouse button 5 is not pressed then aim at closest target
				if (distanceToEnemy > 1.f)
				{
					// Movement prediction
					float dx = targetEnemy.x + targetEnemy.velocityX * 5 - x;
					float dy = targetEnemy.y + targetEnemy.velocityY * 5 - y;

					Vector2* vec = window.GetWindowPosition();

					// Screen is flipped for team 2
					if (playerTeam == 2)
					{
						dx *= -1;
						dy *= -1;
					}

					// change this 69 till your cursor hits exactly on champ
					vec->x = GetSystemMetrics(SM_CXSCREEN) / 2 + dx * offset;
					vec->y = GetSystemMetrics(SM_CYSCREEN) / 2 - dy * offset;

					mouse.executeMovementTo(window, *vec);
					Sleep(50);
				}
			}
		}
		goto windowExists;
	}
windowExists:
	{
		while (window.WindowExists() && !window.WindowFocused()) {
			// derp
			// If the window EXISTS, but isn't focused, then just wait. All hacks turn off aswell.
			// This was a major issue with the original hack lol, if I didn't press Mouse Key 5 WHILE on my second screen,
			// then it would just move my mouse back to the other screen and keep aiming. Was cancerous. Added a toggle instead of hold.
		}
		goto windowFocused;
	}
}