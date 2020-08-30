// ProcessExample1.cpp : Defines the entry point for the console application.
// Memory Sharing between processes using FileMap
//
// this technique uses the pagefile.sys to share a memory region between processes
//
// Dumarey Frederik - 2016

#include"stdafx.h"
#include<Windows.h>
#include<stdio.h>


int main(int argc, char* argv[])
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	HANDLE fileHandle;
	wchar_t ID [] = TEXT("Local\\sharedmemory"); // Global = all users; Local = current user
	char* memory;

	if (1 == argc) // when starting a executable, standard starts with 1 argument with the execution path as value
	{
		fileHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, ID); // create 1024 bytes in the pagefile.sys
		memory = (char*)MapViewOfFile(fileHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0); // map the region in Read/Write to a local variable
		
		sprintf_s(memory, 1024, "%s", "This is some data from the main process"); // fill the region with some data
		printf("Main process: %s\n", memory);
		
		memset(&startupInfo, 0, sizeof(startupInfo)); // fill object with zeroes to be sure all flags are reset
		memset(&processInfo, 0, sizeof(processInfo));
		startupInfo.cb = sizeof(startupInfo);

		char Args[256];
		sprintf_s(Args, "dummyArgs %s", argv[0]);

		CreateProcess(LPCWSTR(argv[0]), LPWSTR(Args), 0, 0, 0, 0, 0, 0, &startupInfo, &processInfo); // launch the same program again with more arguments
		WaitForSingleObject(processInfo.hProcess, INFINITE); // wait for the process handle to finish

		UnmapViewOfFile(memory);
		CloseHandle(fileHandle);
		
	}
	else
	{
		fileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, ID); // get the memory region from the pagefile.sys
		memory = (char*)MapViewOfFile(fileHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		printf("The second process gets the data: %s\n", memory);

		UnmapViewOfFile(memory);
		CloseHandle(fileHandle);
	}

	getchar();

    return 0;
}