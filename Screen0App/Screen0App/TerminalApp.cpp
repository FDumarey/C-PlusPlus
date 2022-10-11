// Terminal Application library
// Uses the console for a VT120 Emulation
// Including a status line and a input routine
//
// (c) 2021 Frederik Dumarey, Belgium

#include <stdio.h>
#include <wchar.h>
#include <windows.h>

#define ESC "\x1b"
#define CSI "\x1b["
#define BEL "\x07"

/*
Hex	ASCII	DEC Line Drawing
0x6a	j	┘
0x6b	k	┐
0x6c	l	┌
0x6d	m	└
0x6e	n	┼
0x71	q	─
0x74	t	├
0x75	u	┤
0x76	v	┴
0x77	w	┬
0x78	x	│
*/

HANDLE gDoneEvent = NULL; // Windows Event
HANDLE hTimer = NULL; // Windows Timer
HANDLE hTimerQueue = NULL; // Timer queue

enum class Colors
{
	fBlack = 30,
	fRed,
	fGreen,
	fYellow,
	fBlue,
	fMagenta,
	fCyan,
	fWhite,
	bBlack = 40,
	bRed,
	bGreen,
	bYellow,
	bBlue,
	bMagenta,
	bCyan,
	bWhite,
	Bold = 1,
	Underline = 4,
	Negative = 7,
};

struct strInput
{
	BOOL ready;
	char* input;
};

void SetColor(const Colors& color)
{
	int i = static_cast<int> (color);
	printf(ESC "[%dm", i);
}

void LocateXY(const int& x, const int& y)
{
	printf(ESC "[%d;%dH", y, x);
}

void PrintHorizontalLine(const int& y)
{
	printf(ESC "(0"); // Enter Line drawing mode
	LocateXY(0, y);
	for (int i = 1; i < 81; i++)
		printf("q"); // Horizontal line
	printf(ESC "(B"); // exit line drawing mode
}

void PrintVerticalLine(const int& x)
{
	printf(ESC "(0"); // Enter Line drawing mode
	for (int i = 1; i < 25; i++)
	{
		LocateXY(x, i);
		printf("x"); // print vertical line
	}
	printf(ESC "(B"); // exit line drawing mode
}

void PrintRectangle(const int& x1, const int& y1, const int& x2, const int& y2)
{
	if (x1 >= x2 || y1 >= y2) return; // Check bounds
	printf(ESC "(0"); //Enter line drawing mode
	int i;
	//Draw upper part
	LocateXY(x1, y1);
	printf("l");
	for (i = x1 + 1; i < x2; i++) printf("q");
	printf("k");
	//Draw lower part
	LocateXY(x1, y2);
	printf("m");
	for (i = x1 + 1; i < x2; i++) printf("q");
	printf("j");
	//Draw left part
	for (i = y1 + 1; i < y2; i++)
	{
		LocateXY(x1, i);
		printf("x");
	}
	//Draw right part
	for (i = y1 + 1; i < y2; i++)
	{
		LocateXY(x2, i);
		printf("x");
	}
	printf(ESC "(B"); // exit line drawing mode
}

BOOL CALLBACK ClearStatusLine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	//Get current cursor location
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi )) return false;
	int oldx = csbi.dwCursorPosition.X+1;
	int oldy = csbi.dwCursorPosition.Y+1;

	//Clear status line
	LocateXY(1, 25);
	printf(CSI "K"); // clear the line
	::SetEvent(gDoneEvent); // signals the event
	CloseHandle(gDoneEvent);
	BOOL succes = ::DeleteTimerQueue(hTimerQueue);

	//Reset cursor
	LocateXY(oldx, oldy);
	return succes;
}

void PrintStatusLine(const char* pszMessage, const int& timer = 0)
{
	LocateXY(1, 25);
	printf(CSI "K"); // clear the line
	printf(pszMessage);
	
	if (timer != 0)
	{
		gDoneEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL); // Creates a Windows event, manual reset, not signaled
		hTimerQueue = ::CreateTimerQueue(); // Creates a timer queue handle
		::CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)ClearStatusLine, NULL, timer * 1000, 0, 0); // Creates the APC
	}
}

void ClearScreen(void)
{
	printf(ESC "[2J");
}

DWORD TerminalInit(void)
{
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	// Get current screen mode and leave this mode
	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		return GetLastError();
	}

	// Enable VT120 special terminal escape characters mode
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		return GetLastError();
	}

	// Set alternate buffer mode
	CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
	GetConsoleScreenBufferInfo(hOut, &ScreenBufferInfo);
	COORD Size;
	Size.X = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
	Size.Y = ScreenBufferInfo.srWindow.Bottom - ScreenBufferInfo.srWindow.Top + 1;
	printf(CSI "?1049h");

	return 0;
}

void TerminalEnd(void)
{
	// Exit the alternate buffer
	printf(CSI "?1049l");
}

void SetTitle(const char* title)
{
	// Set window title
	printf(ESC "]0;%s" BEL , title);
}

void SetCursor(const bool& show, const bool& blink)
{
	if (show) printf(ESC "[?25h"); else printf(ESC "[?25l");
	if (blink) printf(ESC "[?12h"); else printf(ESC "[?12l");
}

strInput GetInput(const int& x, const int& y, const char* inputmask, const BOOL& sync)
{
	//Init local vars
	int position = 0; // Current relative cursor position
	wchar_t key; // Last keypress code
	strInput returnvalue; //Return value
	returnvalue.input = NULL;
	returnvalue.ready = true;
	const int size = (int)strlen(inputmask)+1; // Inputmask size + add a \0
	if (size < 2 ) return returnvalue; //Wrong inputmask
	char* output = new char[size];
	returnvalue.input = new char[size];
	
	//Print Output
	LocateXY(x, y);
	
	//Clean input mask to output
	memcpy(output, inputmask, size);
	for (int i = 0; i < size-1; i++)
	{
		char& temp = output[i];
		if (temp == '#')
		{
			temp = '0';
		}
		else if (temp == '*')
		{
			temp = '_';
		}
		printf("%c", temp);
	}

	//Set cursor blink and start location for input
	SetCursor(true, true);
	for (int i = 0; i < size-1; i++)
	{
		if (inputmask[i] == '#' || inputmask[i] == '*')
		{
			position = i;
			break;
		}
	}
		
	//Loop until return pressed
	do
	{
		if (position == size-1) position=size-2; //Check out of bounds
		LocateXY(x + position, y); //Set the new cursor location
		
		key = _getwch(); //Get key press
		
		if ((int)key == 0xE0)
		{
			key = _getwch(); //Ignore 0xE0 and get special char
			//Home key to begin
			if ((int)key == 71) {
				for (int i = 0; i < size-1; i++)
				{
					if (inputmask[i] == '#' || inputmask[i] == '*')
					{
						position = i;
						break;
					}
				}
			}
			//End key to end
			else if ((int)key == 79) {
				for (int i = size-1; i > 0; i--)
				{
					if (inputmask[i] == '#' || inputmask[i] == '*')
					{
						position = i;
						break;
					}
				}
			}
			//Left key
			else if ((int)key == 75 && position > 1) {
				for (int i = position-1; i > 0; i--)
				{
					if (inputmask[i] == '#' || inputmask[i] == '*')
					{
						position = i;
						break;
					}
				}
			}
			//Right key
			else if ((int)key == 77 && position < size-2) {
				for (int i = position+1; i < size-1; i++)
				{
					if (inputmask[i] == '#' || inputmask[i] == '*')
					{
						position = i;
						break;
					}
				}
			}
			//Insert key
			else if ((int)key == 82) {
				int stop; //Gets last input position
				char stopchar = inputmask[position];
				for (stop = position; stop < size - 2; stop++)
				{
					if (inputmask[stop] != stopchar)
					{
						stop--;
						break;
					}
				}
				//Shift existing input to the right
				for (int i = stop; i > position; i--)
				{
					output[i] = output[i - 1];
					LocateXY(x + i, y);
					printf("%c",output[i]);
				}
				LocateXY(x + position, y);
				if (stopchar == '*') {
					printf("_");
					output[position] = '_';
				}
				else {
					printf("0");
					output[position] = '0';
				}
			}
			//Delete key
			else if ((int)key == 83) {
				int stop; //Gets last input position
				char stopchar = inputmask[position];
				for (stop = position; stop < size - 2; stop++)
				{
					if (inputmask[stop] != stopchar)
					{
						stop--;
						break;
					}
				}
				//Shift existing input to the right
				for (int i = position; i < stop+1; i++)
				{
					output[i] = output[i + 1];
					LocateXY(x + i, y);
					printf("%c", output[i]);
				}
				LocateXY(x + stop, y);
				if (stopchar == '*') {
					printf("_");
					output[stop] = '_';
				}
				else {
					printf("0");
					output[stop] = '0';
				}
				LocateXY(x + position, y);
			}
		}
		
		else if (inputmask[position] == '*') //Check for text input
		{
			//Allow space
			if ((int)key == 32) {
				printf("%c", key);
				output[position] = key;
				position++;
			}
			//Allow backspace
			if ((int)key == 8 && position > 1) {
				for (int i = position - 1; i > 0; i--)
				{
					if (inputmask[i] == '*')
					{
						position = i;
						LocateXY(x + position, y);
						printf("_");
						output[position] = '_';
						break;
					}
				}
			}
			//Allow capital letters
			else if (key >= 'A' && key <= 'Z') {
				printf("%c", key);
				output[position] = key;
				position++;
			}
			//Allow small letters
			else if (key >= 'a' && key <= 'z') {
				printf("%c", key);
				output[position] = key;
				position++;
			}
		}

		else if (inputmask[position] == '#') //Check for numeric input
		{
			//Allow backspace
			if ((int)key == 8 && position > 1) {
				for (int i = position - 1; i > 0; i--)
				{
					if (inputmask[i] == '#')
					{
						position = i;
						LocateXY(x + position, y);
						printf("0");
						output[position] = '0';
						break;
					}
				}
			}
			//Allow numbers
			if (key >= '0' && key <= '9') {
				printf("%c", key);
				output[position] = key;
				position++;
			}
		}
	} while ((int)key != 13);
	
	memcpy(returnvalue.input, output, size);
	returnvalue.ready = true;
	return returnvalue;
}

int main()
{
	wchar_t wch; //keypress var
		
	// Init screen0 and set title
	DWORD succes = TerminalInit();
	SetTitle("Terminal title");
	
	// Set borders
	PrintHorizontalLine(1);
	PrintVerticalLine(1);
	PrintHorizontalLine(24);
	PrintVerticalLine(80);

	// Set Rectangle
	PrintRectangle(10, 10, 20, 20);

	// Try some Set Graphics Rendition (SGR) terminal escape sequences
	LocateXY(2, 4);
	SetColor(Colors::fRed);
	printf("This text has a red foreground using SGR.31.\r\n");
	LocateXY(2, 6);
	SetColor(Colors::bYellow);
	printf("This text shows the foreground and background change at the same time.\r\n");
	PrintStatusLine("Enter text in the input routine",10);

	// Input routine
	strInput pwd = GetInput(5, 5, "Password:****####", true);
	LocateXY(5, 6);
	printf("The output is:%s", pwd.input);
		
	SetCursor(false, false);
	PrintStatusLine("Press any key to exit");
	wch = _getwch(); // wait for key press before exiting
	TerminalEnd();

	return 0;
}