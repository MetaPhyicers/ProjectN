#include "GUI.h"
#include "Windows.h"

class Pannel* g_pPannel = nullptr;

class Pannel
{
public:
	Pannel(Window* window)
		:gui(new ControlsManager(window->GetRenderer())), window(window)
	{
		g_pPannel = this;

		fps = gui->CreateStaticText(L"fps", 0.f, 0.f);
		fps->SetPosition(Widgets::StaticText::TEXT_POS_RIGHT_TOP);

		glm::vec4 textColor = glm::vec4(1.f, 0.5f, 0.2f, 1.f);
		fps->SetColor(textColor);
	}

	~Pannel()
	{
		SafeDelete(gui);
	}

	void Update(float dt)
	{
		gui->Update(dt);

		std::wstring fpsText = L"FPS:" + std::to_wstring(Window::GetFPS());
		fps->SetTitle(fpsText);
	}

	ControlsManager* gui;
	Window* window;
	Widgets::StaticText* fps;

};
