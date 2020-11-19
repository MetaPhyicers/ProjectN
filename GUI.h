#pragma once
#include "Widgets.h"
#include "Event.h"

/**
*	\brief: Provide user interface, such as button, text, box, manage and dispatch messages to all controls
*/
class ControlsManager
{
public:
	ControlsManager(Graphic::Renderer* renderer);
	~ControlsManager();

	// Create widgets
	Widgets::StaticText* CreateStaticText(const wchar_t* title, float x, float y, Widgets::StaticText::TextStyle style = Widgets::StaticText::TEXT_STYLE_NORMAL);
	Widgets::Button* CreateButton(const wchar_t* title,float x, float y, float width, float height, Widgets::Button::ButtonStyle style = Widgets::Button::BUTTON_RECTANGLE);


	void Update(float dt);
	void KeyInputGLFW(Event::EventAction action, uint32_t keyValue);
	void MouseInputGLFW(int button, int action, long xPos, long yPos, long dx, long dy);

private:	
	std::list<Widgets::BasicWidget*> controlsList;
	Widgets::BasicWidget* focusControl;
	Widgets::BasicWidget* mouseDockedControl;
	Graphic::Renderer* renderer;
	EventsQueue* eventsQueue;

	void SetFocus(Widgets::BasicWidget* control);
	const Widgets::BasicWidget* GetFocus();
	void SetCursorDockedControl(Widgets::BasicWidget* control);
	const Widgets::BasicWidget* GetCursorDockedContol();

	friend class Widgets::StaticText;
	friend class Widgets::Button;
};

class GUIFactory
{
public:
	GUIFactory();
	~GUIFactory();


};