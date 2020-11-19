#pragma once
#include "Utility.h"

namespace Widgets
{
	class BasicWidget;
}

class Event
{
public:

	enum class EventType
	{
		EVENT_MOUSE = 0,
		EVENT_KEYBOARD,
		EVENT_SYSTEM,
		EVENT_UNKNOWN
	};

	enum class EventAction
	{
		ACTION_LBUTTON_DOWN = 0, // mouse left button click
		ACTION_LBUTTON_RELEASE, // mouse left button release
		ACTION_LBUTTON_DBCLICK, // moue left button double click
		ACTION_RBUTTON_DOWN, // mouse right button click
		ACTION_RBUTTON_RELEASE, // mouse right button release
		ACTION_RBUTTON_DBCLICK, // mouse right button double click

		ACTION_CURSOR_MOVE,  // cursor move
		ACTION_CURSOR_DOCK, // cursor dock
		ACTION_CURSOR_LEAVE, // cursor leave

		ACTION_KEY_PRESS, // key press
		ACTION_KEY_RELEASE, // key release

		ACTION_WINDOW_SIZE_CHANGE, // window size change

		ACTION_UNKNOWN // unknown action
	};

	typedef void (*ActionHandler) (Widgets::BasicWidget* widget, const Event& evt);

	Event();
	virtual ~Event();

	const Event::EventType GetEventType() const;
	const Event::EventAction GetEventAction() const;
	

protected:
	EventType type;
	EventAction action;

	friend class ControlsManager;
};

class KeyEvent : public Event
{
public:
	KeyEvent();
	~KeyEvent();

	const inline unsigned int GetKeyCode() const { return keyCode; }

private:
	unsigned int keyCode;
	void SetAction(Event::EventAction action);

	friend class ControlsManager;
};

class MouseEvent : public Event
{
public:
	MouseEvent();
	~MouseEvent();

	const inline int GetXPos() const { return xPos; }
	const inline int GetYPos() const { return yPos; }
	const inline int GetZPos() const { return zPos; }

private:
	int xPos;
	int yPos;
	int zPos;
	void SetAction(Event::EventAction action);

	friend class ControlsManager;
};

/**
*	System event is a broadcast event
*/
class SytemEvent : public Event
{
public:
	SytemEvent();
	~SytemEvent();

private:
	void SetAction(Event::EventAction action);

	friend class ControlsManager;
};

/*
*	\description: events queue
*/
class EventsQueue
{
public:
	EventsQueue();
	~EventsQueue();

	Event& Front();
	Event& Back();
	size_t Count();
	bool Empty();
	void Pop();
	void Push(const KeyEvent& evt);
	void Push(const MouseEvent& evt);

private:
	/**
	*	list node declare
	*/
	struct Node
	{
		~Node() {
			SafeDelete(evt);
		}

		Node* pre;
		Node* next;
		Event* evt; ///< mixed list
	};

	Node* head;
	Node* tail;
	size_t count;

	Node* unusedMouseEventsList;
	Node* unusedKeyEvenstList;	
};