//#include <cstdio>
#include <vector>
#include <windows.h>
#include <tlhelp32.h>
#include "memory.h"

DWORD GetProcessId(const wchar_t* processName) { 

	uintptr_t processId = 0;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(processName, procEntry.szExeFile))
				{
					processId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);

	return processId;
}

uintptr_t GetModuleBaseAddress(DWORD processId, const wchar_t* moduleName) { 
	

	uintptr_t moduleBase = 0;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 moduleEntry;
		moduleEntry.dwSize = sizeof(moduleEntry);

		if (Module32First(hSnap, &moduleEntry))
		{
			do
			{
				if (!_wcsicmp(moduleName, moduleEntry.szModule))
				{
					moduleBase = (uintptr_t)moduleEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &moduleEntry));
		}
	}
	CloseHandle(hSnap);

	return moduleBase;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t localPtr, std::vector<unsigned int> offsets) {

	uintptr_t address = localPtr;

	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)address, &address, sizeof(address), 0);
		address += offsets[i];
	}

	return address;
}