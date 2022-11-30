#pragma once
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>

DWORD GetProcessId(const wchar_t* processName);

uintptr_t GetModuleBaseAddress(DWORD processId, const wchar_t* moduleName);

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t localPtr, std::vector<unsigned int> offsets);

namespace Memory {
	
	template <typename T>
	constexpr T Read(HANDLE hProc, const uintptr_t& address)
	{
		T value = {};

		::ReadProcessMemory(hProc, reinterpret_cast<const void*>(address), &value, sizeof(T), 0);
		return value;
	}

	template <typename T>
	constexpr void Write(HANDLE hProc, const uintptr_t& address, const T& value)
	{
		::WriteProcessMemory(hProc, reinterpret_cast<const void*>(address), &value, sizeof(T), 0);
	}
}