#include "MemoryManager.h"
#include <iostream>

bool getPID(const char* ProcessName, DWORD& PID)
{
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry);

	do
		if (!strcmp((const char*)ProcEntry.szExeFile, ProcessName))
		{
			PID = ProcEntry.th32ProcessID;
			CloseHandle(hPID);

			return true;
		}
	while (Process32Next(hPID, &ProcEntry));

	return false;
}

void grabHandle(DWORD PID, HANDLE& processHandle)
{
	std::cout << "PID " << (int)PID << std::endl;
	std::cout << "Grabbing handle.." << std::endl;
	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	if (processHandle == INVALID_HANDLE_VALUE || !processHandle) {
		std::cerr << "Failed to open process -- invalid handle!" << std::endl;
		std::cerr << "Error code: " << GetLastError() << std::endl;
		return;
	}
	else {
		std::cout << "Successfully grabbed handle.." << std::endl;
	}
}

MemoryManager::MemoryManager()
{

	getPID(BATTLERITE_EXE.c_str(), PID);
	
	handle = NULL;
	grabHandle(PID, handle);

	HANDLE moduleSnapshotHandle_ = INVALID_HANDLE_VALUE;
	moduleSnapshotHandle_ = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (moduleSnapshotHandle_ == INVALID_HANDLE_VALUE)
	{
		std::cout << "Module Snapshot error" << std::endl;
		return;
	}

	/* Size the structure before usage */
	MODULEENTRY32 moduleEntry_;
	moduleEntry_.dwSize = sizeof(MODULEENTRY32);

	/* Retrieve information about the first module */
	if (!Module32First(moduleSnapshotHandle_, &moduleEntry_))
	{
		std::cout << "First module not found" << std::endl;
		CloseHandle(moduleSnapshotHandle_);
		return;
	}

	/* Get base addresses of modules */
	Battlerite_Base = NULL;
	MonoDll_Base = NULL;
	while (!Battlerite_Base || !MonoDll_Base)
	{
		/* Find module of the executable */
		do
		{
			if (!strcmp((const char*)moduleEntry_.szModule, BATTLERITE_EXE.c_str()))
			{
				Battlerite_Base = (unsigned int)moduleEntry_.modBaseAddr;
			}
			if (!strcmp((const char*)moduleEntry_.szModule, MONO_DLL.c_str()))
			{
				MonoDll_Base = (unsigned int)moduleEntry_.modBaseAddr;
			}
		} while (Module32Next(moduleSnapshotHandle_, &moduleEntry_));

		if (!Battlerite_Base)
		{
			std::cout << "Failed to find module " << BATTLERITE_EXE << std::endl;
			Sleep(200);
		}
		else if (!MonoDll_Base)
		{
			std::cout << "Failed to find module " << MONO_DLL << std::endl;
			Sleep(200);
		}
	}
	std::cout << BATTLERITE_EXE << " = " << Battlerite_Base << std::endl;
	std::cout << MONO_DLL << " = " << MonoDll_Base << std::endl;

}
