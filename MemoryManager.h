#pragma once
#ifndef _MEMORY_MANAGER_H
#define _MEMORY_MANAGER_H

#include <Windows.h>
#include <tlhelp32.h>
#include <string>

class MemoryManager
{
private:
	DWORD PID;		// process ID
	const std::string BATTLERITE_EXE = "Battlerite.exe";
	const std::string MONO_DLL = "mono.dll";
public:
	HANDLE handle;
	DWORD Battlerite_Base;
	DWORD Battlerite_Size;
	DWORD MonoDll_Base;
	DWORD MonoDLL_Size;

	MemoryManager();
};

#endif