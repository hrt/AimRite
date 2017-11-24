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

const static DWORD OFFSET_COOLDOWNS[] = { 0x001F5994, 0x0, 0x4C, 0x764, 0x368 };
const static DWORD OFFSET_COOLDOWNS_R = 0x19C;
const static DWORD OFFSET_COOLDOWNS_Q = 0x104;
const static DWORD OFFSET_COOLDOWNS_SPACE = 0xB8;
const static DWORD OFFSET_COOLDOWNS_E = 0x150;
const static DWORD OFFSET_COOLDOWNS_RIGHT = 0x6C;

const static DWORD OFFSET_LOCAL_BUTTONS[] = { 0x0105365C, 0x590, 0x54, 0xF0, 0x5A8 };
const static DWORD OFFSET_LOCAL_ALPHA = 0x32C;
const static DWORD OFFSET_LOCAL_NUMERIC = 0x324;
const static DWORD OFFSET_LOCAL_MOUSE = 0x348;

const static DWORD OFFSET_ENTITY_LIST[] = { 0x001F55EC, 0x40, 0xB4, 0x98 };
const static DWORD PLAYER_SIZE = 0x2C;
const static DWORD OFFSET_ENTITY_START = 0x214;
const static DWORD OFFSET_ENTITY_DIRECTION_X = 0x28;
const static DWORD OFFSET_ENTITY_DIRECTION_Y = 0x2C;
const static DWORD OFFSET_ENTITY_X = 0x20;
const static DWORD OFFSET_ENTITY_Y = 0x24;
const static DWORD OFFSET_ENTITY_TEAM = 0x18;

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