#include "GGConsoleGraphics.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cassert>

static WORD ConsoleColorToAttribute(const GG::ConsoleColor color)
{
	switch (color)
	{
		case GG::ConsoleColor::Black:		return 0;
		case GG::ConsoleColor::Red:			return FOREGROUND_RED | FOREGROUND_INTENSITY;
		case GG::ConsoleColor::Green:		return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		case GG::ConsoleColor::Blue:		return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		case GG::ConsoleColor::Yellow:		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		case GG::ConsoleColor::Magenta:		return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		case GG::ConsoleColor::Cyan:		return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		case GG::ConsoleColor::White:		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		case GG::ConsoleColor::DarkGray:	return FOREGROUND_INTENSITY;
		case GG::ConsoleColor::DarkRed:		return FOREGROUND_RED;
		case GG::ConsoleColor::DarkGreen:	return FOREGROUND_GREEN;
		case GG::ConsoleColor::DarkBlue:	return FOREGROUND_BLUE;
		case GG::ConsoleColor::DarkYellow:	return FOREGROUND_RED | FOREGROUND_GREEN;
		case GG::ConsoleColor::DarkMagenta:	return FOREGROUND_RED | FOREGROUND_BLUE;
		case GG::ConsoleColor::DarkCyan:	return FOREGROUND_GREEN | FOREGROUND_BLUE;
		case GG::ConsoleColor::Gray:		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	}

	assert(false);
	return {};
}

static WORD ToBackgroundColor(const WORD color) { return static_cast<WORD>(color << 4); }

static uint8_t s_KeyMap[0xFF] = {};
static bool s_PrevKeyState[0xFF];
static bool s_KeyState[0xFF];

static bool s_PrevMouseState[8];
static bool s_MouseState[8];
static SHORT s_MouseX;
static SHORT s_MouseY;
static float s_ScrollDelta;

static void CreateKeyMap()
{
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Backspace)]		= VK_BACK;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Tab)]			= VK_TAB;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Return)]			= VK_RETURN;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Shift)]			= VK_SHIFT;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Control)]		= VK_CONTROL;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Alt)]			= VK_MENU;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Pause)]			= VK_PAUSE;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::CapsLock)]		= VK_CAPITAL;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Escape)]			= VK_ESCAPE;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Space)]			= VK_SPACE;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::PageUp)]			= VK_PRIOR;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::PageDown)]		= VK_NEXT;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::End)]			= VK_END;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Home)]			= VK_HOME;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::LeftArrow)]		= VK_LEFT;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::UpArrow)]		= VK_UP;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::RightArrow)]		= VK_RIGHT;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::DownArrow)]		= VK_DOWN;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::PrintScreen)]	= VK_SNAPSHOT;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Insert)]			= VK_INSERT;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Delete)]			= VK_DELETE;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num0)]			= 0x30;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num1)]			= 0x31;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num2)]			= 0x32;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num3)]			= 0x33;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num4)]			= 0x34;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num5)]			= 0x35;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num6)]			= 0x36;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num7)]			= 0x37;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num8)]			= 0x38;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Num9)]			= 0x39;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::A)]				= 0x41;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::B)]				= 0x42;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::C)]				= 0x43;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::D)]				= 0x44;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::E)]				= 0x45;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F)]				= 0x46;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::G)]				= 0x47;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::H)]				= 0x48;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::I)]				= 0x49;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::J)]				= 0x4A;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::K)]				= 0x4B;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::L)]				= 0x4C;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::M)]				= 0x4D;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::N)]				= 0x4E;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::O)]				= 0x4F;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::P)]				= 0x50;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Q)]				= 0x51;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::R)]				= 0x52;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::S)]				= 0x53;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::T)]				= 0x54;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::U)]				= 0x55;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::V)]				= 0x56;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::W)]				= 0x57;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::X)]				= 0x58;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Y)]				= 0x59;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Z)]				= 0x5A;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::LeftSuper)]		= VK_LWIN;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::RightSuper)]		= VK_RWIN;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad0)]		= VK_NUMPAD0;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad1)]		= VK_NUMPAD1;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad2)]		= VK_NUMPAD2;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad3)]		= VK_NUMPAD3;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad4)]		= VK_NUMPAD4;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad5)]		= VK_NUMPAD5;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad6)]		= VK_NUMPAD6;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad7)]		= VK_NUMPAD7;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad8)]		= VK_NUMPAD8;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::Keypad9)]		= VK_NUMPAD9;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::KeypadMultiply)]	= VK_MULTIPLY;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::KeypadAdd)]		= VK_ADD;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::KeypadSubtract)]	= VK_SUBTRACT;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::KeypadPeriod)]	= VK_DECIMAL;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::KeypadDivide)]	= VK_DIVIDE;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F1)]				= VK_F1;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F2)]				= VK_F2;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F3)]				= VK_F3;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F4)]				= VK_F4;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F5)]				= VK_F5;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F6)]				= VK_F6;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F7)]				= VK_F7;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F8)]				= VK_F8;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F9)]				= VK_F9;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F10)]			= VK_F10;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F11)]			= VK_F11;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F12)]			= VK_F12;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F13)]			= VK_F13;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F14)]			= VK_F14;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F15)]			= VK_F15;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F16)]			= VK_F16;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F17)]			= VK_F17;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F18)]			= VK_F18;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F19)]			= VK_F19;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F20)]			= VK_F20;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F21)]			= VK_F21;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F22)]			= VK_F22;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F23)]			= VK_F23;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::F24)]			= VK_F24;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::NumLock)]		= VK_NUMLOCK;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::ScrollLock)]		= VK_SCROLL;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::LeftShift)]		= VK_LSHIFT;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::RightShift)]		= VK_RSHIFT;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::LeftControl)]	= VK_LCONTROL;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::RightControl)]	= VK_RCONTROL;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::LeftAlt)]		= VK_LMENU;
	s_KeyMap[static_cast<uint8_t>(GG::KeyCode::RightAlt)]		= VK_RMENU;
}

bool GG::Input::GetKeyDown(const KeyCode key)
{
	const uint8_t virtualKey = s_KeyMap[static_cast<uint8_t>(key)];
	return s_KeyState[virtualKey] == true && s_PrevKeyState[virtualKey] == false;
}

bool GG::Input::GetKeyUp(const KeyCode key)
{
	const uint8_t virtualKey = s_KeyMap[static_cast<uint8_t>(key)];
	return s_KeyState[virtualKey] == false && s_PrevKeyState[virtualKey] == true;
}

bool GG::Input::GetKey(const KeyCode key)
{
	const uint8_t virtualKey = s_KeyMap[static_cast<uint8_t>(key)];
	return s_KeyState[virtualKey];
}

bool GG::Input::GetMouseButtonDown(const MouseButton button)
{
	return s_MouseState[static_cast<uint8_t>(button)] == true && s_PrevMouseState[static_cast<uint8_t>(button)] == false;
}

bool GG::Input::GetMouseButtonUp(const MouseButton button)
{
	return s_MouseState[static_cast<uint8_t>(button)] == false && s_PrevMouseState[static_cast<uint8_t>(button)] == true;
}

bool GG::Input::GetMouseButton(const MouseButton button)
{
	return s_MouseState[static_cast<uint8_t>(button)];
}

uint8_t GG::Input::GetMouseX() { return static_cast<uint8_t>(s_MouseX); }
uint8_t GG::Input::GetMouseY() { return static_cast<uint8_t>(s_MouseY); }
float GG::Input::GetScrollDelta() { return s_ScrollDelta; }

static constexpr wchar_t FULL_BLOCK = 0x2588;

GG::ConsoleApp::ConsoleApp(
	const std::wstring& title,
	const uint16_t bufferWidth,
	const uint16_t bufferHeight,
	const uint8_t scale,
	const uint8_t ups)
	:
	m_Title(title),
	m_MSPerTick(1000 / ups)
{
	// arbitrary minimums found before size explodes. Don't know what sets these
	assert(bufferWidth >= 15);
	assert(bufferHeight >= 2);
	assert(scale > 0);

	CreateKeyMap();

	AllocConsole();

	//TODO probably create new console buffer?
	m_BufferHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	m_InputHandle = GetStdHandle(STD_INPUT_HANDLE);

	SetConsoleMode(m_InputHandle, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT);

	SetConsoleTitle(title.c_str());

	ShowCursor(false);

	SMALL_RECT viewport = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(m_BufferHandle, TRUE, &viewport);

	Sleep(100); //TODO find proper fix for this

	COORD size = {};
	size.X = static_cast<SHORT>(bufferWidth);
	size.Y = static_cast<SHORT>(bufferHeight);
	SetConsoleScreenBufferSize(m_BufferHandle, size);

	CONSOLE_FONT_INFOEX consoleFont = {};
	consoleFont.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	consoleFont.nFont = 0;
	consoleFont.dwFontSize.X = static_cast<SHORT>(8 * scale);
	consoleFont.dwFontSize.Y = static_cast<SHORT>(8 * scale);
	consoleFont.FontFamily = FF_DONTCARE;
	consoleFont.FontWeight = FW_DONTCARE;
	wcscpy_s(consoleFont.FaceName, L"Terminal");
	SetCurrentConsoleFontEx(m_BufferHandle, FALSE, &consoleFont);

	CONSOLE_SCREEN_BUFFER_INFO currentScreenInfo = {};
	GetConsoleScreenBufferInfo(m_BufferHandle, &currentScreenInfo);
	m_BufferWidth = currentScreenInfo.dwSize.X;
	m_BufferHeight = currentScreenInfo.dwSize.Y;
	m_Buffer = new CHAR_INFO[m_BufferWidth * m_BufferHeight];

	viewport = {};
	viewport.Left = 0;
	viewport.Top = 0;
	viewport.Right = static_cast<SHORT>(currentScreenInfo.dwMaximumWindowSize.X - 1);
	viewport.Bottom = static_cast<SHORT>(currentScreenInfo.dwMaximumWindowSize.Y - 1);
	SetConsoleWindowInfo(m_BufferHandle, TRUE, &viewport);

	Sleep(100);

	const HWND hwnd = GetConsoleWindow();

	SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU);
	ShowWindow(hwnd, SW_SHOW);

	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);
	const LONG windowWidth = windowRect.right - windowRect.left;
	const LONG windowHeight = windowRect.bottom - windowRect.top;

	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	const LONG centeredX = screenWidth / 2 - windowWidth / 2;
	const LONG centeredY = screenHeight / 2 - windowHeight / 2;
	MoveWindow(hwnd, centeredX, centeredY, windowWidth, windowHeight, TRUE);

	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_TimerFrequency));
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_StartTime));
}

GG::ConsoleApp::~ConsoleApp()
{
	delete[] m_Buffer;
}

void GG::ConsoleApp::Run()
{
	Init();

	uint64_t now;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
	double lastTime = static_cast<double>(now - m_StartTime) / m_TimerFrequency;

	while (true)
	{
		SetBackgroundColor(m_ClearColor);
		SetForegroundColor(ConsoleColor::White);
		Clear();

		// do input stuff
		INPUT_RECORD inputBuffer[256];
		DWORD eventsRead;
		PeekConsoleInput(m_InputHandle, inputBuffer, 256, &eventsRead);
		if (eventsRead > 0)
		{
			FlushConsoleInputBuffer(m_InputHandle);
			for (uint32_t i = 0; i < eventsRead; i++)
				InputProc(inputBuffer[i]);
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
		double time = static_cast<double>(now - m_StartTime) / m_TimerFrequency;
		const double dt = time - lastTime;
		lastTime = time;

		Update(static_cast<float>(dt));
		Draw(static_cast<float>(dt));

		memcpy(s_PrevKeyState, s_KeyState, sizeof(s_KeyState));
		memcpy(s_PrevMouseState, s_MouseState, sizeof(s_MouseState));
		s_ScrollDelta = 0;

		// present equivalent
		COORD bufferCoord = {};
		bufferCoord.X = m_BufferWidth;
		bufferCoord.Y = m_BufferHeight;

		SMALL_RECT rect = {};
		rect.Right = m_BufferWidth - 1;
		rect.Bottom = m_BufferHeight - 1;

		WriteConsoleOutput(m_BufferHandle, m_Buffer, bufferCoord, { 0, 0 }, &rect);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
		time = static_cast<double>(now - m_StartTime) / m_TimerFrequency;
		const uint64_t updateTimeMS = static_cast<uint64_t>((time - lastTime) * 1000);
		const int16_t sleepTime = max(m_MSPerTick - updateTimeMS, 0);
		Sleep(sleepTime);
	}
}

void GG::ConsoleApp::SetClearColor(const ConsoleColor color)
{
	m_ClearColor = color;
}

void GG::ConsoleApp::SetBackgroundColor(const ConsoleColor color)
{
	m_BackgroundColor = ToBackgroundColor(ConsoleColorToAttribute(color));
}

void GG::ConsoleApp::SetForegroundColor(const ConsoleColor color)
{
	m_ForegroundColor = ConsoleColorToAttribute(color);
}

void GG::ConsoleApp::ShowCursor(const bool visible)
{
	CONSOLE_CURSOR_INFO cursorInfo = {};
	GetConsoleCursorInfo(m_BufferHandle, &cursorInfo);
	cursorInfo.bVisible = static_cast<BOOL>(visible);

	SetConsoleCursorInfo(m_BufferHandle, &cursorInfo);
}

void GG::ConsoleApp::PutText(const wchar_t* text, const uint16_t x, const uint16_t y, const bool truncate)
{
	assert(x < m_BufferWidth);
	assert(y < m_BufferHeight);

	if (x >= m_BufferWidth || y >= m_BufferHeight)
		return;

	const uint32_t start = y * m_BufferWidth + x;
	const uint32_t bufferLength = m_BufferWidth * m_BufferHeight;

	for (uint32_t i = 0; text[i] != L'\0'; i++)
	{
		if (start + i >= bufferLength)
			break;

		if (truncate && x + i >= m_BufferWidth)
			break;

		UpdateBuffer(start + i, text[i]);
	}
}

void GG::ConsoleApp::PutChar(const wchar_t character, const uint16_t x, const uint16_t y)
{
	assert(x < m_BufferWidth);
	assert(y < m_BufferHeight);

	if (x >= m_BufferWidth || y >= m_BufferHeight)
		return;

	const uint32_t index = y * m_BufferWidth + x;
	UpdateBuffer(index, character);
}

void GG::ConsoleApp::PutPixel(const uint16_t x, const uint16_t y)
{
	PutPixel(x, y, FULL_BLOCK);
}

void GG::ConsoleApp::PutPixelBackground(const uint16_t x, const uint16_t y)
{
	PutPixel(x, y, L' ');
}

void GG::ConsoleApp::PutPixel(const uint16_t x, const uint16_t y, const wchar_t character)
{
	assert(x < m_BufferWidth);
	assert(y < m_BufferHeight);

	if (x >= m_BufferWidth || y >= m_BufferHeight)
		return;

	const uint32_t index = y * m_BufferWidth + x;
	UpdateBuffer(index, character);
}

void GG::ConsoleApp::PutRectFill(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height)
{
	PutRectFill(x, y, width, height, FULL_BLOCK);
}

void GG::ConsoleApp::PutRectFillBackground(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height)
{
	PutRectFill(x, y, width, height, L' ');
}

void GG::ConsoleApp::PutRectFill(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, const wchar_t character)
{
	assert(x < m_BufferWidth);
	assert(x + width <= m_BufferWidth);
	assert(y < m_BufferHeight);
	assert(y + height <= m_BufferHeight);

	if (x >= m_BufferWidth || y >= m_BufferHeight)
		return;

	uint16_t right = x + width;
	uint16_t bottom = y + height;

	right = min(right, m_BufferWidth);
	bottom = min(bottom, m_BufferHeight);

	for (uint32_t row = y; row < bottom; row++)
	for (uint32_t col = x; col < right; col++)
	{
		const uint32_t index = row * m_BufferWidth + col;
		UpdateBuffer(index, character);
	}
}

void GG::ConsoleApp::Clear()
{
	CHAR_INFO blank = {};
	blank.Char.UnicodeChar = L' ';
	blank.Attributes = m_ForegroundColor | m_BackgroundColor;

	std::fill(&m_Buffer[0], &m_Buffer[m_BufferWidth * m_BufferHeight], blank);
}

void GG::ConsoleApp::UpdateBuffer(const uint32_t index, const wchar_t character)
{
	m_Buffer[index].Char.UnicodeChar = character;
	m_Buffer[index].Attributes = m_ForegroundColor | m_BackgroundColor;
}

void GG::ConsoleApp::InputProc(const INPUT_RECORD record)
{
	switch (record.EventType)
	{
	case KEY_EVENT:
		const KEY_EVENT_RECORD keyEvent = record.Event.KeyEvent;
		s_KeyState[keyEvent.wVirtualKeyCode] = keyEvent.bKeyDown;
		break;

	case MOUSE_EVENT:
		const MOUSE_EVENT_RECORD mouseEvent = record.Event.MouseEvent;
		s_MouseX = mouseEvent.dwMousePosition.X;
		s_MouseY = mouseEvent.dwMousePosition.Y;
		if (mouseEvent.dwEventFlags & MOUSE_WHEELED)
		{
			s_ScrollDelta = static_cast<short>(HIWORD(mouseEvent.dwButtonState)) / static_cast<float>(WHEEL_DELTA);
		}
		break;

	case FOCUS_EVENT:
		const FOCUS_EVENT_RECORD focusEvent = record.Event.FocusEvent;
		if (focusEvent.bSetFocus == FALSE)
		{
			memset(s_KeyState, false, sizeof(s_KeyState));
			memset(s_PrevKeyState, false, sizeof(s_PrevKeyState));
			memset(s_MouseState, false, sizeof(s_MouseState));
			memset(s_PrevMouseState, false, sizeof(s_PrevMouseState));
		}
		break;

	case WINDOW_BUFFER_SIZE_EVENT:
	case MENU_EVENT:
		break;
	}
}
