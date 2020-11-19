#include "GUI.h"
#include "Windows.h"


ControlsManager::ControlsManager(Graphic::Renderer* renderer)
	:controlsList(), focusControl(nullptr), renderer(renderer), eventsQueue(new EventsQueue())
{
}

ControlsManager::~ControlsManager()
{
	SafeDelete(eventsQueue);
}

Widgets::StaticText* ControlsManager::CreateStaticText(const wchar_t* title, float x, float y, Widgets::StaticText::TextStyle style)
{
	Widgets::StaticText* staticText = new Widgets::StaticText(title, x, y, style);
	staticText->Init(); 
	staticText->SetControlsManager(this);


	///< set external interface
	focusControl = staticText;

	renderer->AddObeject(staticText); ///< append it into render target list
	controlsList.push_back(staticText);

	return staticText;
}

Widgets::Button* ControlsManager::CreateButton(const wchar_t* title, float x, float y, float width, float height, Widgets::Button::ButtonStyle style)
{
	Widgets::Button* button = new Widgets::Button(title, x, y, width, height, style);
	button->Init();
	button->SetControlsManager(this);

	///< set external interface
	focusControl = button;

	renderer->AddObeject(button);
	controlsList.push_back(button);

	return button;
}

void ControlsManager::SetFocus(Widgets::BasicWidget* control)
{
	focusControl = control;
}

const Widgets::BasicWidget* ControlsManager::GetFocus()
{
	return focusControl;
}

void ControlsManager::SetCursorDockedControl(Widgets::BasicWidget* control)
{
	mouseDockedControl = control;
}

const Widgets::BasicWidget* ControlsManager::GetCursorDockedContol()
{
	return mouseDockedControl;
}

void ControlsManager::Update(float dt)
{
	/**
	*	dispatch messages and calling external proccess controls action on depth first
	*/ 

	for (std::list<Widgets::BasicWidget*>::reverse_iterator control = controlsList.rbegin(); 
		control != controlsList.rend(); control++) {
		if (eventsQueue->Empty()) {
			break;
		}
		if ((*control)->Confirm(eventsQueue->Front())) {
			eventsQueue->Pop();
		}
	}

	if (!eventsQueue->Empty()) {
		eventsQueue->Pop();
	}


	for (std::list<Widgets::BasicWidget*>::iterator iter = controlsList.begin(); 
		iter != controlsList.end(); ++iter) {
		(*iter)->Update(dt);
	}

}

void ControlsManager::KeyInputGLFW(Event::EventAction action, uint32_t keyValue)
{
	KeyEvent keyEvent;
	keyEvent.action = action;
	keyEvent.keyCode = keyValue;

	eventsQueue->Push(keyEvent);
}

void ControlsManager::MouseInputGLFW(int button, int action, long xPos, long yPos, long dx, long dy)
{
	MouseEvent mouseEvent;
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			mouseEvent.action = Event::EventAction::ACTION_LBUTTON_DOWN;
		}
		else if(action == GLFW_RELEASE) {
			mouseEvent.action = Event::EventAction::ACTION_LBUTTON_RELEASE;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			mouseEvent.action = Event::EventAction::ACTION_RBUTTON_DOWN;
		}
		else if (action == GLFW_RELEASE) {
			mouseEvent.action = Event::EventAction::ACTION_RBUTTON_RELEASE;
		}
	}

	mouseEvent.xPos = xPos;
	mouseEvent.yPos = static_cast<long>(Window::GetWindowHeight()) - yPos;

	eventsQueue->Push(mouseEvent);
}
