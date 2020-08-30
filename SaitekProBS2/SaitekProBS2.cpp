// SaitekProBS2.cpp : Defines the entry point for the console application.
//

#undef UNICODE

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "DirectOutputImpl.h"
#include <vector>
#include <process.h>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>

#pragma comment (lib,"Ws2_32.lib")

//Prepare winsock server variables
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

enum LedID {
	FireButton = 0,
	FireARed,
	FireAGreen,
	FireBRed,
	FireBGreen,
	FireDRed,
	FireDGreen,
	FireERed,
	FireEGreen,
	Toggle1Red,
	Toggle1Green,
	Toggle3Red,
	Toggle3Green,
	Toggle5Red,
	Toggle5Green,
	Pov2Red,
	Pov2Green,
	ClutchRed,
	ClutchGreen,
	ThrottleAxis
};

enum StringID {
	TopLine = 0,
	MiddleLine,
	BottomLine
};

enum LEDStatus
{
	On = 0,
	Off,
	Blinking
};

int blink = 0; //toggle blinking state for leds
static void* g_device = 0; //pointer to the first Saitek device
int scrl = 0; //wheel scroll integer
int page = 0; //current page
void utt(void* dummy); //interface for user thread

void __stdcall devicechange(void* device, bool added, void* ctxt)
{
	if (g_device == 0 && added)
		g_device = device;
	else if (!added && g_device == device)
		g_device = 0;
}

void __stdcall enumeratedevice(void* device, void* ctxt)
{
	g_device = device;
}

void __stdcall softbutton(void* device, DWORD btns, void* ctxt)
{
	switch (btns)
		{
		case 0x2:
			if (scrl > 0) scrl--;
			printf("Scroll changed to: %d\n", scrl);
			break;
		case 0x4:
			if (scrl < 3) scrl++;
			printf("Scroll changed to: %d\n", scrl);
			break;
		default:
			return;
			break;
		}
}

void __stdcall pagechange(void* device, DWORD callpage, bool active, void* ctxt)
{
	printf("Page changed to: %d\n", callpage);
	page = callpage;
}

void SetLED(CDirectOutput& member, LedID led, LEDStatus status)
{
	//Led is already known
	//Changing blink status
	for (int i = 0; i < 3; i++){
		switch (status)
		{
		case On:
			member.SetLed(g_device, i, led, 1);
			break;
		case Off:
			member.SetLed(g_device, i, led, 0);
			break;
		case Blinking:
			if (blink == 0) member.SetLed(g_device, i, led, 1);
			else member.SetLed(g_device, i, led, 0);
			break;
		default:
			break;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Start program, write to console to make sure program runs
	char c;
	std::cout << "Black Shark 2 To Saitek X52 Pro programm initialization\n";
	std::cout << "-------------------------------------------------------\n";
	std::cout << "(c)2015 FreSoft\n\n";
	
	// Initialise DirectOutput class object
	std::cout << "Initializing DirectOutput.dll object...\n";
	HRESULT hr; // return value
	CDirectOutput m_directoutput; // create member object
	hr = m_directoutput.Initialize(L"BS22X52"); // create class object and initialize
	if (FAILED(hr))
	{
		std::cout << "DirectOutput initialisation failed!\n";
		goto ErrorHandle;
	}
	else
	{
		std::cout << "Directoutput initialisation succesfull!\n";
	}

	// Register callback functions
	std::cout << "Registering device callback function...\n";
	hr = m_directoutput.RegisterDeviceCallback(devicechange, 0); //register device change callback function
	if (FAILED(hr))
	{
		std::cout << "Device callback function registration failed!\n";
		goto ErrorHandle;
	}
	else
	{
		std::cout << "Device callback function registration succesfull!\n";
	}

	// Enumerate devices
	std::cout << "Searching Saitek X52 Pro hardware...\n";
	hr = m_directoutput.Enumerate(enumeratedevice, 0);
	if (FAILED(hr) || !g_device)
	{
		std::cout << "Saitek X52 Pro device not found!\n";
		goto ErrorHandle;
	}
	else
	{
		std::cout << "Saitek X52 Pro device found!\n";
	}

	// Registering button callback functions
	std::cout << "Registering button callback functions...\n";
	hr = m_directoutput.RegisterPageCallback(g_device, pagechange, 0);
	if (FAILED(hr))
	{
		std::cout << "Page button callback registration failed!\n";
		goto ErrorHandle;
	}
	else
	{
		std::cout << "Page button callback registration succesfull!\n";
	}

	hr = m_directoutput.RegisterSoftButtonCallback(g_device, softbutton, 0);
	if (FAILED(hr))
	{
		std::cout << "Soft button callback registration failed!\n";
		goto ErrorHandle;
	}
	else
	{
		std::cout << "Soft button callback registration succesfull!\n";
	}
	
	// Add pages
	std::cout << "Adding pages to device...\n";
	hr = m_directoutput.AddPage(g_device, 0, L"General", TRUE);
	hr = m_directoutput.AddPage(g_device, 1, L"Coords", FALSE);
	hr = m_directoutput.AddPage(g_device, 2, L"Route", FALSE);
	if (FAILED(hr))
	{
		std::cout << "Page registration failed!\n";
		goto ErrorHandle;
	}
	else
	{
		std::cout << "Page registration succesfull!\n";
	}

	// Setting initial text and leds
	std::cout << "Setting initial Black Shark 2 text and leds...\n";
	for (int i = 0; i < 3; i++)
	{
		hr = m_directoutput.SetLed(g_device, i, FireButton, 1);
		hr = m_directoutput.SetLed(g_device, i, FireARed, 0);
		hr = m_directoutput.SetLed(g_device, i, FireAGreen, 1);
		hr = m_directoutput.SetLed(g_device, i, FireBRed, 0);
		hr = m_directoutput.SetLed(g_device, i, FireBGreen, 1);
		hr = m_directoutput.SetLed(g_device, i, FireDRed, 0);
		hr = m_directoutput.SetLed(g_device, i, FireDGreen, 1);
		hr = m_directoutput.SetLed(g_device, i, FireERed, 0);
		hr = m_directoutput.SetLed(g_device, i, FireEGreen, 1);
		hr = m_directoutput.SetLed(g_device, i, Toggle1Red, 0);
		hr = m_directoutput.SetLed(g_device, i, Toggle1Green, 1);
		hr = m_directoutput.SetLed(g_device, i, Toggle3Red, 0);
		hr = m_directoutput.SetLed(g_device, i, Toggle3Green, 1);
		hr = m_directoutput.SetLed(g_device, i, Toggle5Red, 0);
		hr = m_directoutput.SetLed(g_device, i, Toggle5Green, 1);
		hr = m_directoutput.SetLed(g_device, i, Pov2Red, 0);
		hr = m_directoutput.SetLed(g_device, i, Pov2Green, 1);
		hr = m_directoutput.SetLed(g_device, i, ClutchRed, 0);
		hr = m_directoutput.SetLed(g_device, i, ClutchGreen, 1);
		hr = m_directoutput.SetLed(g_device, i, ThrottleAxis, 1);
	}
	hr = m_directoutput.SetString(g_device, 0, TopLine, 13, L"Black Shark 2");
	hr = m_directoutput.SetString(g_device, 0, MiddleLine, 18, L"to Saitek X52 Pro");
	hr = m_directoutput.SetString(g_device, 0, BottomLine, 15, L"(c)2015 FreSoft");
	std::cout << "Entering the main loop...\n";
		
	//Main loop
	HANDLE w = (HANDLE)_beginthread(utt, 0, NULL);
	
	//Initialize Winsock
	WSADATA wsaData;
	int iResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("Winsock startup failed with error: %d\n", iResult);
		goto ErrorHandle;
	}
	printf("Winsock startup succesfull!\n");
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Resolve the host adress and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0){
		printf("Host adress and port info failed with error: %d\n", iResult);
		WSACleanup();
		goto ErrorHandle;
	}
	printf("Host adress and port information succesfull!\n");

	//Create a socket
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET){
		printf("Socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		goto ErrorHandle;
	}
	printf("Socket creation succesfull!\n");

	//Setup listening
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR){
		printf("Bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		goto ErrorHandle;
	}
	printf("Bind succesfull!\n");
	freeaddrinfo(result);
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR){
		printf("Listener failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		goto ErrorHandle;
	}
	printf("Listener succesfully installed, waiting for an incoming connection...!\n");
	
	//Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET){
		printf("Client acceptance failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		goto ErrorHandle;
	}
	printf("Client socket acceptance operational!\n");
	closesocket(ListenSocket);

	//Setting main loop variables
	bool loop = TRUE; //loop variable
	DWORD ws; //return value for thread signaling
	
	//Loop
	while (loop)
	{
		//Listen to TCP socket for information
		ZeroMemory(&recvbuf, DEFAULT_BUFLEN);
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Network bytes received: %d, data received: %s\n", iResult, recvbuf);
		}
		else if (iResult == 0) {
			printf("Connection closed by sender!\n");
			loop = FALSE;
		}

		//Set leds
		SetLED(m_directoutput, FireAGreen, Blinking);
		
		//Set text
		
		//Wait for user thread to end
		ws = WaitForSingleObject(w, 0); //just check thread signaled value
		if (ws != WAIT_TIMEOUT) loop = FALSE; //is thread still busy?
		
		//Wait do perform I/O ops
		Sleep(50);
		if (blink == 0) blink = 1; else blink = 0;
	}


	ErrorHandle:
	//Termination, wait for keypress to stop process & main thread
	std::cout << "Terminating programm...\n";
	hr = m_directoutput.Deinitialize();
	iResult = shutdown(ClientSocket, SD_SEND);
	closesocket(ClientSocket);
	WSACleanup();
	c = std::cin.get();
	return 0;
}

void utt(void* dummy)
{
	std::cout << "Press any key to exit\n";
	getchar();
}