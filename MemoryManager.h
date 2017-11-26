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
	DWORD MonoDll_Base;
  template<class c>
  c Read(DWORD dwAddress)
  {
    c val;
    ReadProcessMemory(handle, (LPVOID)dwAddress, &val, sizeof(c), NULL);
    return val;
  }

  template<class c>
  BOOL Write(DWORD dwAddress, c valueToWrite)
  {
    return WriteProcessMemory(handle, (LPVOID)dwAddress, &valueToWrite, sizeof(c), NULL);
  }
	MemoryManager();
};

#endif