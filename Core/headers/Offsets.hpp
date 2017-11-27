#pragma once
#ifndef _OFFSETS_H
#define _OFFSETS_H
#define _USE_MATH_DEFINES
#include <ctime>
#include <math.h>

#define radiansToDegrees(angleRadians) (angleRadians * 180.0 / M_PI)
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

/* Alpha */
#define MOVE_RIGHT 16
#define MOVE_LEFT 2
#define MOVE_DOWN 524288
#define MOVE_UP 8388608
#define CAST_Q 131072
#define CAST_E 32
#define CAST_R 262144
#define CAST_F 64

/* Numeric */
#define CAST_1 131072
#define CAST_2 262144
#define CAST_SPACE 1

/* Mouse */
#define CAST_LEFT 8
#define CAST_RIGHT 16
#define CAST_MIDDLE 32

const static DWORD OFFSET_LOCAL_PLAYER[] = { 0xF9AE70, 0x38, 0x2F8, 0x598, 0x1C };
const static DWORD OFFSET_LOCAL_X = 0x30;
const static DWORD OFFSET_LOCAL_Y = 0x38;

const static DWORD OFFSET_ENTITY_LIST[] = { 0x001F55EC, 0x40, 0xB4, 0x98 };
const static DWORD ENTITY_SIZE = 0x2C;
const static DWORD OFFSET_ENTITY_START = 0x214;

const static DWORD OFFSET_CHAMPION_LIST[] = { 0x001F5994, 0x4, 0x40, 0x4B4, 0x110 };
const static DWORD CHAMPION_SIZE = 0x13C;
const static DWORD OFFSET_CHAMPION_START = 0x0;

// Used to read from champion list, corresponds to battlerites struct
struct ChampionInformation
{
	DWORD padding0[6];		// 0
	float x;				// 18
	float y;				// 1C
	DWORD padding20[7];		// 20
	int team;				// 38
	DWORD padding3C[7];		// 3C
	float currentHP;		// 58
	DWORD padding5C[11];	// 5C
	float currentEnergy;	// 88
	DWORD padding4;			// 8C
	float maxEnergy;		// 90
	DWORD padding94[15];	// 94
	float maxHP;			// D0
};

// Used to read from entity list, corresponds to battlerites struct
struct EntityInformation
{
	DWORD padding0[6];		// 0
	int team;				// 18
	DWORD padding1C;		// 1C
	float x;				// 20
	float y;				// 24
	float directionX;		// 28
	float directionY;		// 2C
};

// Used to load information from champions
struct PlayerInformation
{
	float x;
	float y;
	float previousX;
	float previousY;
	float velocityX;
	float velocityY;
	clock_t lastUpdate;
};

#endif