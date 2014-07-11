// LoLHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>

enum ErrorCodes {
	LOLH_ERROR_SUCCESS = 0x0,
	LOLH_ERROR_OPENING_PROCESS,
	LOLH_ERROR_ALLOCATING_MEMORY,
	LOLH_ERROR_WRITING_MEMORY,
	LOLH_ERROR_CREATING_THREAD,
	LOLH_ERROR_FETCHING_THREAD_EXITCODE,
	LOLH_ERROR_LOADING_DLL,
	LOLH_ERROR_RUNNING_INIT
};

DWORD FindTargetProcessID(std::wstring windowName) {
	DWORD pid;
	HWND hWnd; // Handle to the main window of the target process

	std::cout << "Finding process ID...\n";
	// Find the window so we can get the process ID
	hWnd = FindWindowW(NULL, windowName.c_str());
	GetWindowThreadProcessId(hWnd, &pid);
	std::cout << "Process ID = '" << pid << "'\n";
	return pid;
}

HANDLE OpenTargetProcess(DWORD pid) {
	DWORD processAccessFlags = PROCESS_ALL_ACCESS;
	HANDLE hProcess;

	// Open the process
	std::cout << "Opening target process...\n";
	hProcess = OpenProcess(processAccessFlags, FALSE, pid);
	if (hProcess == NULL) {
		std::cerr << "Error: Process could not be opened successfully. Error code: '" << GetLastError() << "'\n";
		return NULL;
	}
	std::cout << "Process opened successfully.\n";

	return hProcess;
}

void* AllocMemoryInTargetProcess(HANDLE hProcess, std::wstring moduleName) {
	void* pRemoteModuleName = NULL;
	SIZE_T modNameLen = moduleName.length() + 1;

	std::cout << "Allocating memory for DLL name in target process memory...\n";

	// Reserve memory for the module name
	pRemoteModuleName = VirtualAllocEx(hProcess, NULL, modNameLen * 2, MEM_COMMIT, PAGE_READWRITE);
	if (pRemoteModuleName == NULL) {
		std::cerr << "Error: Could not write target DLL name to target process memory successfully. Error code: '" << GetLastError() << "'\n";
		return NULL;
	}
	return pRemoteModuleName;
}

BOOL WriteDLLNameToTargetProcess(HANDLE hProcess, std::wstring moduleName, void* pRemoteModuleName) {
	SIZE_T modNameLen = moduleName.length() + 1;
	SIZE_T numBytesWritten = 0;

	std::cout << "Writing DLL name to target process memory...\n";
	BOOL result = WriteProcessMemory(hProcess, pRemoteModuleName,
		(void*)moduleName.c_str(), modNameLen * 2, &numBytesWritten);
	if (numBytesWritten != (modNameLen * 2)) {
		std::cerr << "Error: did not write proper amount of bytes to target process. Tried to write '" <<
			(modNameLen * 2) << "' but only wrote '" << numBytesWritten << "'\n";
		return NULL;
	}
	else if (!result) {
		std::cerr << "Error: could not write memory in target process. Error code: '" << GetLastError() << "'\n";
		return NULL;
	}
	std::cout << "Target DLL name successfully written to process memory.\n";
	return result;
}

DWORD LoadLibraryInRemoteProcess(HANDLE hProcess, void* pRemoteModuleName, DWORD modNameLen) {
	HMODULE hKernel32 = GetModuleHandle(L"Kernel32");
	DWORD hLoadedLibModule = NULL;

	// Create a thread to call LoadLibraryW and load our DLL into
	// the target process
	std::cout << "Creating new thread in target process running LoadLibraryW...\n";

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW"),
		pRemoteModuleName, 0, NULL);
	if (hThread == NULL) {
		std::cerr << "Error: could not create remote thread in target process. Error code: '" << GetLastError() << "'\n";
		return hLoadedLibModule;
	}
	WaitForSingleObject(hThread, INFINITE);

	BOOL result = GetExitCodeThread(hThread, &hLoadedLibModule);
	if (!result) {
		std::cerr << "Error: could not get remote thread exit code. Error code: '" << GetLastError() << "'\n";
		hLoadedLibModule = NULL;
	}
	else if (!hLoadedLibModule) {
		std::cerr << "Error: thread created, but could not load module properly.\n";
	}
	else {
		printf_s("Thread created and finished successfully. Return value = '0x%X'\n", hLoadedLibModule);
	}
	CloseHandle(hThread);
	return hLoadedLibModule;
}

int RunInitInRemoteProcess(HANDLE hProcess, DWORD initFuncAddress) {
	DWORD exitCode = NULL;

	// Create the thread to run our initialization function - DllMain has many limitations
	// on what it can do, we circumvent this here.
	std::cout << "Creating new thread in target process running InitDll...\n";
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)(initFuncAddress), 0, 0, NULL);
	if (hThread == NULL) {
		std::cerr << "Error: could not create remote thread in target process. Error code: '" << GetLastError() << "'\n";
		return exitCode; 
	}
	WaitForSingleObject(hThread, INFINITE);

	BOOL result = GetExitCodeThread(hThread, &exitCode);
	if (!result) {
		std::cerr << "Error: could not get remote thread exit code. Error code: '" << GetLastError() << "'\n";
		exitCode = NULL;
	}
	else if (!exitCode) {
		std::cerr << "Error: thread created, but could not load module properly.\n";
	}
	else {
		std::cout << "Thread created and finished successfully. Return value = '" << exitCode << "'\n";
	}
	CloseHandle(hThread);
	return exitCode;
}

int HookLoLProcess(std::wstring fullDllPath, DWORD initDllRelAddr) {
	std::wstring windowName(L"League of Legends (TM) Client");
	//std::wstring windowName(L"Camera Demo");
	std::wstring moduleName = fullDllPath;
	SIZE_T modNameLen = moduleName.length() + 1;

	DWORD hLoadedLibModule = NULL; // Base address of the loaded module (handle)
	void* pRemoteModuleName = NULL; // The address in the remote process where
	// the remote module name will be copied
	DWORD pid = 0; // Process ID of the target process
	HANDLE hProcess = NULL; // Handle to the target process
	BOOL result;

	pid = FindTargetProcessID(windowName);

	hProcess = OpenTargetProcess(pid);
	if (!hProcess)
		return LOLH_ERROR_OPENING_PROCESS;

	pRemoteModuleName = AllocMemoryInTargetProcess(hProcess, moduleName);
	if (!pRemoteModuleName) {
		CloseHandle(hProcess);
		return LOLH_ERROR_ALLOCATING_MEMORY;
	}

	result = WriteDLLNameToTargetProcess(hProcess, moduleName, pRemoteModuleName);
	if (!result) {
		VirtualFreeEx(hProcess, pRemoteModuleName, modNameLen * 2, MEM_RELEASE);
		CloseHandle(hProcess);
		return LOLH_ERROR_WRITING_MEMORY;
	}

	hLoadedLibModule = LoadLibraryInRemoteProcess(hProcess, pRemoteModuleName, modNameLen);
	if (!hLoadedLibModule) {
		VirtualFreeEx(hProcess, pRemoteModuleName, modNameLen * sizeof(TCHAR), MEM_RELEASE);
		CloseHandle(hProcess);
		return LOLH_ERROR_LOADING_DLL;
	}

	int initResult = RunInitInRemoteProcess(hProcess, hLoadedLibModule + initDllRelAddr);
	if (initResult <= 0) {
		VirtualFreeEx(hProcess, pRemoteModuleName, modNameLen * sizeof(TCHAR), MEM_RELEASE);
		CloseHandle(hProcess);
		return LOLH_ERROR_RUNNING_INIT;
	}

	// Clean up
	VirtualFreeEx(hProcess, pRemoteModuleName, modNameLen * sizeof(TCHAR), MEM_RELEASE);
	CloseHandle(hProcess);

	std::cout << "Cleaned up and returned successfully.\n";
	return 0;
}

typedef int (CALLBACK* LOLHOOKINIT)(LPCWSTR);

TCHAR* GetPathNameFromFileName(TCHAR* path) {
	TCHAR* curChar = path + (lstrlenW(path) - 1);

	// Loop from the end to the beginning of the string or until
	// we find a path separator
	for (; curChar > path; curChar--) {
		if (curChar[0] == '\\') {
			curChar[1] = NULL;
			break;
		}
	}
	return path;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hDLL = NULL;
	LOLHOOKINIT lolHookInit = NULL;
	DWORD initDllRelAddr = NULL;
	
	hDLL = LoadLibrary(L"LoLHookDLL");
	if (hDLL != NULL) {
		lolHookInit = (LOLHOOKINIT)GetProcAddress(hDLL, "_InitDll@4");
		if (!lolHookInit)
			return GetLastError();
		 initDllRelAddr = reinterpret_cast<DWORD>(lolHookInit) - 
			reinterpret_cast<DWORD>(hDLL);
		FreeLibrary(hDLL);

		std::wstring dllPath = GetPathNameFromFileName(argv[0]);
		std::wstring dllName = L"LoLHookDLL.dll";
		return HookLoLProcess(dllPath + dllName, initDllRelAddr);
	}
	return 1;
}

