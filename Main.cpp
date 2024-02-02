#include "GGConsoleGraphics.h"

class App : public GG::ConsoleApp
{
protected:
	void Init() override
	{
		SetClearColor(GG::ConsoleColor::Magenta);

		m_PlayerX = GetWidth() / 2;
		m_PlayerY = GetHeight() / 2;
	}

	void Update(float deltaTime) override
	{
		if (GG::Input::GetKey(GG::KeyCode::D))
			m_PlayerX += m_Speed * deltaTime;
		if (GG::Input::GetKey(GG::KeyCode::A))
			m_PlayerX -= m_Speed * deltaTime;
		if (GG::Input::GetKey(GG::KeyCode::S))
			m_PlayerY += m_Speed * deltaTime;
		if (GG::Input::GetKey(GG::KeyCode::W))
			m_PlayerY -= m_Speed * deltaTime;

		if (m_PlayerX >= GetWidth())
			m_PlayerX = 0;
		if (m_PlayerX < 0)
			m_PlayerX = GetWidth();
		if (m_PlayerY >= GetHeight())
			m_PlayerY = 0;
		if (m_PlayerY < 0)
			m_PlayerY = GetHeight();
	}

	void Draw(float deltaTime) override
	{
		wchar_t text[128];
		swprintf_s(text, L"%dx%d", (uint16_t)m_PlayerX, (uint16_t)m_PlayerY);
		PutText(text, 0, GetHeight() - 1);

		swprintf_s(text, L"%dx%d", GG::Input::GetMouseX(), GG::Input::GetMouseY());
		PutText(text, 0, 0);

		SetForegroundColor(GG::ConsoleColor::Green);
		PutPixel(m_PlayerX, m_PlayerY);
	}

private:
	static constexpr float m_Speed = 5;
	float m_PlayerX;
	float m_PlayerY;
};

int main()
{
	auto* app = new App;
	app->Run();
	delete app;
}