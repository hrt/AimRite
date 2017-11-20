#pragma once
#ifndef _OFFSETS_H
#define _OFFSETS_H
#include <ctime>

const static DWORD OFFSET_LOCAL_PLAYER[] = { 0xF9AE70, 0x38, 0x2F8, 0x598, 0x1C };
const static DWORD OFFSET_LOCAL_X = 0x30;
const static DWORD OFFSET_LOCAL_Y = 0x38;

const static DWORD OFFSET_ENTITY_LIST[] = { 0x001F55EC, 0x40, 0xB4, 0x98 };
const static DWORD PLAYER_SIZE = 0x2C;
const static DWORD OFFSET_PLAYER_START = 0x214;
const static DWORD OFFSET_PLAYER_X = 0x20;
const static DWORD OFFSET_PLAYER_Y = 0x24;
const static DWORD OFFSET_PLAYER_TEAM = 0x18;

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