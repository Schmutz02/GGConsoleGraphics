#pragma once

#include <cstdint>
#include <string>

struct _CHAR_INFO;
typedef _CHAR_INFO CHAR_INFO;
typedef unsigned short WORD;
typedef void* HANDLE;
struct _INPUT_RECORD;
typedef _INPUT_RECORD INPUT_RECORD;

namespace GG
{
	enum class MouseButton
	{
		Left,
		Right,
		Middle
	};

	enum class KeyCode
	{
		Space,
		Apostrophe,
		Comma,
		Minus,
		Period,
		Slash,
		Shift,
		Control,
		Alt,
		Num0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		Semicolon,
		Equal,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		LeftBracket,
		Backslash,
		RightBracket,
		Tilde,
		World1,
		World2,
		Escape,
		Return,
		Tab,
		Backspace,
		Insert,
		Delete,
		RightArrow,
		LeftArrow,
		DownArrow,
		UpArrow,
		PageUp,
		PageDown,
		Home,
		End,
		CapsLock,
		ScrollLock,
		NumLock,
		PrintScreen,
		Pause,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,
		Keypad0,
		Keypad1,
		Keypad2,
		Keypad3,
		Keypad4,
		Keypad5,
		Keypad6,
		Keypad7,
		Keypad8,
		Keypad9,
		KeypadPeriod,
		KeypadDivide,
		KeypadMultiply,
		KeypadSubtract,
		KeypadAdd,
		KeypadEnter,
		KeypadEqual,
		LeftShift,
		LeftControl,
		LeftAlt,
		LeftSuper,
		RightShift,
		RightControl,
		RightAlt,
		RightSuper,
		Menu,
		LastCount,
	};

	namespace Input
	{
		bool GetKeyDown(KeyCode key);
		bool GetKeyUp(KeyCode key);
		bool GetKey(KeyCode key);

		bool GetMouseButtonDown(MouseButton button);
		bool GetMouseButtonUp(MouseButton button);
		bool GetMouseButton(MouseButton button);

		uint8_t GetMouseX();
		uint8_t GetMouseY();
		float GetScrollDelta();
	}

	enum class ConsoleColor
	{
		Black,
		Red,
		Green,
		Blue,
		Yellow,
		Magenta,
		Cyan,
		White,
		DarkGray,
		DarkRed,
		DarkGreen,
		DarkBlue,
		DarkYellow,
		DarkMagenta,
		DarkCyan,
		Gray
	};

	class ConsoleApp
	{
	public:
		ConsoleApp(
			const std::wstring& title = L"GGConsoleApp",
			const uint16_t bufferWidth = 20,
			const uint16_t bufferHeight = 20,
			const uint8_t scale = 2,
			const uint8_t ups = 30);

		virtual ~ConsoleApp();

		void Run();

		const std::wstring& GetTitle() const { return m_Title; }
		uint16_t GetWidth() const { return m_BufferWidth; }
		uint16_t GetHeight() const { return m_BufferHeight; }

	protected:
		void SetClearColor(ConsoleColor color);
		void SetBackgroundColor(ConsoleColor color);
		void SetForegroundColor(ConsoleColor color);

		void ShowCursor(bool visible);

		void PutText(const wchar_t* text, uint16_t x, uint16_t y, bool truncate = false);
		void PutChar(wchar_t character, uint16_t x, uint16_t y);

		void PutPixel(uint16_t x, uint16_t y);
		void PutPixelBackground(uint16_t x, uint16_t y);
		void PutRectFill(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
		void PutRectFillBackground(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

		virtual void Init() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Draw(float deltaTime) = 0;

	private:
		void Clear();
		void PutPixel(uint16_t x, uint16_t y, wchar_t character);
		void PutRectFill(uint16_t x, uint16_t y, uint16_t width, uint16_t height, wchar_t character);
		void UpdateBuffer(uint32_t index, wchar_t character);
		void InputProc(INPUT_RECORD record);

		std::wstring m_Title;

		uint16_t m_BufferWidth;
		uint16_t m_BufferHeight;

		CHAR_INFO* m_Buffer;

		HANDLE m_BufferHandle;
		HANDLE m_InputHandle;

		uint16_t m_MSPerTick;
		uint64_t m_StartTime;
		uint64_t m_TimerFrequency;

		ConsoleColor m_ClearColor;
		WORD m_BackgroundColor;
		WORD m_ForegroundColor;
	};
}
