/*Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9
*/

#pragma once
#pragma comment(lib,"winmm.lib")

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode.
#endif

#include <Windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

class ConsoleGameEngine
{
protected:

	struct SKeyState
	{
		bool bPressed;
		bool bRealeased;
		bool bHeld;
	};

protected:

	//SETTINGS
	std::wstring m_appName;

	//SCREEN
	int m_screenWidth{ 0 };
	int m_screenHeight{ 0 };

	SMALL_RECT m_rectWindow;

	CHAR_INFO *m_bufScreen;

	//CONSOLES
	HANDLE m_originalConsole;
	HANDLE m_console;
	HANDLE m_consoleIn;

	CONSOLE_SCREEN_BUFFER_INFO m_originalConsoleInfo;

	bool bIsConsoleInFocus;

	//INPUT & MOUSE
	short m_keyOldState[256];
	short m_keyNewState[256];

	bool m_mouseOldState[5];
	bool m_mouseNewState[5];

	SKeyState m_keys[256];
	SKeyState m_mouse[5];

	int m_mousePosX;
	int m_mousePosY;

	static std::atomic<bool> m_bAtomActive;
	static std::mutex m_muxGame;
	static std::condition_variable m_GameFinished;

private:
	void GameThread()
	{
		//Create User for this thread
		if (!OnUserCreate())
		{
			m_bAtomActive = false;
		}

		auto lTime1 = std::chrono::system_clock::now();
		auto lTime2 = std::chrono::system_clock::now();

		while (m_bAtomActive)
		{
			while (m_bAtomActive)
			{
				//Timer
				float lElapsedTime = HandleTiming(lTime1,lTime2);

				//Handle Input Keys
				HandleKeyboardInput();

				//Handle Mouse Event
				HandleMouseEvent();

				//Handle Frame User Update
				HandleFrameUpdate(lElapsedTime);

				//Update Console Info
				UpdateScreenBuffer(lElapsedTime);
			}
		}
	}

	float HandleTiming(std::chrono::system_clock::time_point& Timer1, std::chrono::system_clock::time_point& Timer2)
	{
		Timer2 = std::chrono::system_clock::now();
		std::chrono::duration<float> lElapsedTimeChrono = Timer2 - Timer1;
		Timer1 = Timer2;

		return lElapsedTimeChrono.count();
	}

	void HandleKeyboardInput()
	{
		for (int i = 0; i < 256; i++)
		{
			auto& lKeyNewState = m_keyNewState[i];
			auto& lKeyOldState = m_keyOldState[i];
			auto& lKey = m_keys[i];

			lKeyNewState = GetAsyncKeyState(i);

			lKey.bPressed = false;
			lKey.bRealeased = false;

			if (lKeyNewState != lKeyOldState)
			{
				if (lKeyNewState == 0x8000)
				{
					lKey.bPressed = !m_keys[i].bHeld;
					lKey.bHeld = true;
				}
				else
				{
					lKey.bRealeased = true;
					lKey.bHeld = false;
				}
			}

			lKeyOldState = lKeyNewState;
		}
	}

	void HandleMouseEvent()
	{
		INPUT_RECORD lInBuf[32];
		DWORD lEvents = 0;

		GetNumberOfConsoleInputEvents(m_consoleIn, &lEvents);

		if (lEvents > 0)
		{
			ReadConsoleInput(m_consoleIn, lInBuf, lEvents, &lEvents);
		}

		for (DWORD i = 0; i < lEvents; i++)
		{
			INPUT_RECORD& lRecord = lInBuf[i];

			switch (lRecord.EventType)
			{
			case FOCUS_EVENT:
				bIsConsoleInFocus = lRecord.Event.FocusEvent.bSetFocus;
				break;

			case MOUSE_EVENT:
				switch (lRecord.Event.MouseEvent.dwEventFlags)
				{
				case MOUSE_MOVED:
					m_mousePosX = lRecord.Event.MouseEvent.dwMousePosition.X;
					m_mousePosY = lRecord.Event.MouseEvent.dwMousePosition.Y;
					break;
				case 0:
					for (int j = 0; j < 5; j++)
					{
						m_mouseNewState[j] = (lRecord.Event.MouseEvent.dwButtonState & (1 << j)) > 0;
					}
					break;
				default:
					break;
				}
				break;
			default:
				break;
				//No Matters For now
			}
		}

		for (int k = 0; k < 5; k++)
		{
			auto& lMouseNewState = m_mouseNewState[k];
			auto& lMouseOldState = m_mouseOldState[k];
			auto& lMouse = m_mouse[k];

			lMouse.bPressed = false;
			lMouse.bRealeased = false;

			if (lMouseNewState != lMouseOldState)
			{
				if (lMouseNewState)
				{
					lMouse.bPressed = true;
					lMouse.bHeld = true;
				}
				else
				{
					lMouse.bRealeased = true;
					lMouse.bHeld = false;
				}
			}

			lMouseOldState = lMouseNewState;
		}
	}

	void HandleFrameUpdate(float ElapsedTime)
	{
		if (!OnUserUpdate(ElapsedTime))
		{
			m_bAtomActive = false;
		}
	}

	void UpdateScreenBuffer(float ElapsedTime)
	{
		wchar_t lTitle[256];

		swprintf_s(lTitle, 256, L"Console Game Engine - %s -FPS: %3.2f", m_appName.c_str(), 1.f / ElapsedTime);

		SetConsoleTitle(lTitle);
		WriteConsoleOutput(m_console, m_bufScreen, { (short)m_screenWidth,(short)m_screenHeight }, { 0,0 }, &m_rectWindow);
	}

	void HandleOnDestroyUser()
	{
		//if override, user could release resources
		if (OnUserDestroy())
		{
			//User has permitted destroy, exit and clean up
			delete[] m_bufScreen;
			SetConsoleActiveScreenBuffer(m_originalConsole);
			m_GameFinished.notify_one();
		}
		else
		{
			//user denied destroy, continue;
			m_bAtomActive = true;
		}
	}
protected:

	int Error(const wchar_t* msg)
	{
		wchar_t buf[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		SetConsoleActiveScreenBuffer(m_originalConsole);
		wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
		return 0;
	}

	static BOOL CloseHandler(DWORD event)
	{
		if (event == CTRL_CLOSE_EVENT)
		{
			m_bAtomActive = false;

			std::unique_lock<std::mutex> lUlock(m_muxGame);
			m_GameFinished.wait(lUlock);
		}

		return true;
	}
public:
	ConsoleGameEngine()
	{
		m_screenWidth = 90;
		m_screenHeight = 30;

		m_console = GetStdHandle(STD_OUTPUT_HANDLE);
		m_consoleIn = GetStdHandle(STD_INPUT_HANDLE);

		std::memset(m_keyNewState,0,256 * sizeof(short));
		std::memset(m_keyOldState,0,256 * sizeof(short));
		std::memset(m_keys,0,256 * sizeof(SKeyState));

		m_mousePosX = 0;
		m_mousePosY = 0;

		m_appName = L"Default";
	}

	~ConsoleGameEngine()
	{
		SetConsoleActiveScreenBuffer(m_originalConsole);
		delete[] m_bufScreen;
	}

	int ConstructConsole(int Width, int Height, int Fontw, int Fonth)
	{
		if (m_console == INVALID_HANDLE_VALUE)
		{
			return Error(L"Bad Handle");
		}

		m_screenWidth = Width;
		m_screenHeight = Height;

		m_rectWindow = { 0,0,1,1 };

		SetConsoleWindowInfo(m_console, TRUE, &m_rectWindow);

		COORD lCoord = { (short)m_screenWidth,(short)m_screenHeight };

		//Set Size of the screen
		if (!SetConsoleScreenBufferSize(m_console, lCoord))
		{
			return Error(L"SetConsoleScreenBufferSize");
		}

		//assign screen buffer to the console
		if (!SetConsoleActiveScreenBuffer(m_console))
		{
			return Error(L"SetConsoleActiveScreenBuffer");
		}

		// Set the font size now that the screen buffer has been assigned to the console
		CONSOLE_FONT_INFOEX lCfi;

		lCfi.cbSize = sizeof(lCfi);
		lCfi.nFont = 0;
		lCfi.dwFontSize.X = Fontw;
		lCfi.dwFontSize.Y = Fonth;
		lCfi.FontFamily = FF_DONTCARE;
		lCfi.FontWeight = FW_NORMAL;

		wcscpy_s(lCfi.FaceName, L"BREF");

		if (!SetCurrentConsoleFontEx(m_console, false, &lCfi))
		{
			return Error(L"SetCurrentConsoleFontEx");
		}

		// Check Buffer Info
		// Return error if dimension/fontSize are too large
		CONSOLE_SCREEN_BUFFER_INFO lCsbi;

		if (!GetConsoleScreenBufferInfo(m_console, &lCsbi))
		{
			return Error(L"GetConsoleBufferInfo");
		}
		if (m_screenHeight > lCsbi.dwMaximumWindowSize.Y)
		{
			return Error(L"Screen Height / Font Height too big");
		}
		if (m_screenWidth > lCsbi.dwMaximumWindowSize.X)
		{
			return Error(L"Screen Width / Font Width too big");
		}

		//Set Physical console window size
		m_rectWindow = { 0,0,(short)(m_screenWidth - 1),(short)(m_screenHeight - 1) };

		if (!SetConsoleWindowInfo(m_console, TRUE, &m_rectWindow))
		{
			return Error(L"SetConsoleWindowInfo");
		}

		//Set flags to allow mouse/Input 
		if (!SetConsoleMode(m_consoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		{
			return Error(L"SetConsoleMode");
		}

		//delete on the destructor func
		//cast to long long to avoid warning about size of int 4 byte to CHAR_INFO  8 byte
		m_bufScreen = new CHAR_INFO[static_cast<long long>(m_screenWidth) * static_cast<long long>(m_screenHeight)];
		memset(m_bufScreen, 0, sizeof(CHAR_INFO) * m_screenWidth * m_screenHeight);

		if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandle,TRUE))
		{
			return Error(L"SetConsoleCtrlHandler");
		}

		return 1;
	}

	void Start()
	{
		m_bAtomActive = true;

		//start thread
		std::thread lThread = std::thread(&ConsoleGameEngine::GameThread, this);

		//wait for thread to be existed
		lThread.join();
	}

	void Fill(int xStart, int yStart, int xEnd, int yEnd, short PixelType = PIXEL_TYPE::PIXEL_SOLID, short Color = COLOUR::FG_BLACK)
	{
		Clip(xStart, yStart);
		Clip(xEnd,yEnd);

		for (int x = xStart; x < xEnd; x++)
		{
			for (int y = yStart; y < yEnd; y++)
			{
				Draw(x, y, PixelType, Color);
			}
		}
	}

	void Clip(int& x, int& y)
	{
		x = x < 0 ? 0 : x;
		x = x > m_screenWidth ? m_screenWidth : x;
		y = y < 0 ? 0 : y;
		y = y > m_screenHeight ? m_screenHeight : y;
	}

	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short PixelType = PIXEL_SOLID, short Color = FG_BLACK)
	{
		DrawLine(x1, y1, x2, y2, PixelType, Color);
		DrawLine(x2, y2, x3, y3, PixelType, Color);
		DrawLine(x3, y3, x1, y1, PixelType, Color);
	}

	//Bresenham’s Algo
	void DrawLine(int x1, int y1, int x2, int y2, short PixelType = PIXEL_SOLID, short Color = FG_BLACK)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

		dx = x2 - x1;
		dy = y2 - y1;

		dx1 = std::abs(dx);
		dy1 = std::abs(dy);

		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;

		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}

			Draw(x,y,PixelType,Color);

			for (i = 0; x < xe; i++)
			{
				x = x + 1; //explicite

				if (px < 0)
				{
					px = px + 2 * dy1;
				}
				else
				{
					y = (dx < 0 && dy < 0) || (dx > 0 && dy > 0) ? y + 1 : y - 1;

					px = px + 2 * (dy1 - dx1);
				}
				
				Draw(x,y,PixelType,Color);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}

			Draw(x,y,PixelType,Color);

			for (i = 0; y < ye; i++)
			{
				y = y + 1;

				if (py <= 0)
				{
					py = py + 2 * dx1;
				}
				else
				{
					x = (dx < 0 && dy < 0) || (dx > 0 && dy > 0) ? x + 1 : x - 1;

					py = py + 2 * (dx1 - dy1);
				}

				Draw(x,y,PixelType,Color);
			}
		}
	}

	void FillTriangle(int x1, int y1, int x2, int y2,int x3,int y3, short PixelType = PIXEL_SOLID, short Color = FG_BLACK)
	{
		auto SwapLambda = [](int& x, int& y) {int temp = x; x = y; y = temp; };
		auto DrawLineLambda = [&](int sx, int ex, int ny) {for (int i = sx; i <= ex; i++) Draw(i,ny,PixelType,Color); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (y1 > y2) { SwapLambda(y1, y2); SwapLambda(x1, x2); }
		if (y1 > y3) { SwapLambda(y1, y3); SwapLambda(x1, x3); }
		if (y2 > y3) { SwapLambda(y2, y3); SwapLambda(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting points
		dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y2 - y1);

		dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(y3 - y1);

		if (dy1 > dx1) {   // swap values
			SwapLambda(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2) {   // swap values
			SwapLambda(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			DrawLineLambda(minx, maxx, y);    // Draw line from min to max points found on the y
			// Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;

		}
	next:
		// Second half
		dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1) {   // swap values
			SwapLambda(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			DrawLineLambda(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
	}

	/// <summary>
	/// Get greyscale color
	/// </summary>
	/// <param name="Ratio">0 -> 1</param>
	/// <returns>gradient between Black and white</returns>
	CHAR_INFO GetGreyScaleColor(float Ratio)
	{
		short lBgColor;
		short lFGColor;

		int pixel_bw = (int)(13.0f * Ratio);

		wchar_t lSymbol;

		switch (pixel_bw)
		{
		case 0: lBgColor = BG_BLACK; lFGColor = FG_BLACK; lSymbol = PIXEL_SOLID; break;

		case 1: lBgColor = BG_BLACK; lFGColor = FG_DARK_GREY; lSymbol = PIXEL_QUARTER; break;
		case 2: lBgColor = BG_BLACK; lFGColor = FG_DARK_GREY; lSymbol = PIXEL_HALF; break;
		case 3: lBgColor = BG_BLACK; lFGColor = FG_DARK_GREY; lSymbol = PIXEL_THREEQUARTERS; break;
		case 4: lBgColor = BG_BLACK; lFGColor = FG_DARK_GREY; lSymbol = PIXEL_SOLID; break;

		case 5: lBgColor = BG_DARK_GREY; lFGColor = FG_GREY; lSymbol = PIXEL_QUARTER; break;
		case 6: lBgColor = BG_DARK_GREY; lFGColor = FG_GREY; lSymbol = PIXEL_HALF; break;
		case 7: lBgColor = BG_DARK_GREY; lFGColor = FG_GREY; lSymbol = PIXEL_THREEQUARTERS; break;
		case 8: lBgColor = BG_DARK_GREY; lFGColor = FG_GREY; lSymbol = PIXEL_SOLID; break;

		case 9:  lBgColor = BG_GREY; lFGColor = FG_WHITE; lSymbol = PIXEL_QUARTER; break;
		case 10: lBgColor = BG_GREY; lFGColor = FG_WHITE; lSymbol = PIXEL_HALF; break;
		case 11: lBgColor = BG_GREY; lFGColor = FG_WHITE; lSymbol = PIXEL_THREEQUARTERS; break;
		case 12: lBgColor = BG_GREY; lFGColor = FG_WHITE; lSymbol = PIXEL_SOLID; break;
		default:
			lBgColor = BG_BLACK; lFGColor = FG_BLACK; lSymbol = PIXEL_SOLID;
		}

		CHAR_INFO c;
		c.Attributes = lBgColor | lFGColor;
		c.Char.UnicodeChar = lSymbol;
		return c;
	}

	////// GETTER ///////
	inline int GetScreenWidth() { return m_screenWidth; }
	inline int GetScreenHeight() { return m_screenHeight; }
	inline float GetScreenAspectRatio() { return (float)m_screenWidth / (float)m_screenHeight; }

	//For Override
protected:

	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float ElaspedTime) = 0;
	virtual bool OnUserDestroy() { return true; }

public:

	virtual void Draw(int x, int y, short PixelType = PIXEL_TYPE::PIXEL_SOLID, short Color = COLOUR::FG_BLACK)
	{
		if (x >= 0 && x < m_screenWidth && y >= 0 && y < m_screenHeight)
		{
			m_bufScreen[y * m_screenWidth + x].Char.UnicodeChar = PixelType;
			m_bufScreen[y * m_screenWidth + x].Attributes = Color;
		}
	}
};

// Define our static variables
std::atomic<bool> ConsoleGameEngine::m_bAtomActive(false);
std::condition_variable ConsoleGameEngine::m_GameFinished;
std::mutex ConsoleGameEngine::m_muxGame;