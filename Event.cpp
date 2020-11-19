#include "Event.h"
#include <stdexcept>

Event::Event()
	:type(EventType::EVENT_UNKNOWN), action(EventAction::ACTION_UNKNOWN)
{
}

Event::~Event()
{
}

const Event::EventType Event::GetEventType() const
{
	return type;
}

const Event::EventAction Event::GetEventAction() const
{
	return action;
}

KeyEvent::KeyEvent()
	:Event(), keyCode(0)
{
	type = EventType::EVENT_KEYBOARD;
}

KeyEvent::~KeyEvent()
{
}

void KeyEvent::SetAction(Event::EventAction action)
{
	this->action = action;
}

MouseEvent::MouseEvent()
	:xPos(0), yPos(0), zPos(0)
{
	type = EventType::EVENT_MOUSE;
}

MouseEvent::~MouseEvent()
{
}

void MouseEvent::SetAction(Event::EventAction action)
{
	this->action = action;
}

EventsQueue::EventsQueue()
	: head(nullptr), tail(nullptr), 
	unusedMouseEventsList(nullptr), unusedKeyEvenstList(nullptr), count(0)
{
}

EventsQueue::~EventsQueue()
{
	/**
	*	clear all event nodes
	*/
	auto release = [&](Node* head)->void{
		Node* temp = head;
		while (temp != nullptr) {
			head = head->next;
			SafeDelete(temp);
			temp = head;
		}
	};

	release(head);
	release(unusedMouseEventsList);
	release(unusedKeyEvenstList);
}

Event& EventsQueue::Front()
{
	return *(head->evt);
}

Event& EventsQueue::Back()
{
	return *(tail->evt);
}

size_t EventsQueue::Count()
{
	return count;
}

bool EventsQueue::Empty()
{
	return count == 0;
}

void EventsQueue::Pop()
{
	if (head == nullptr) {
		return;
	}

	/**
	*	pop unused element to unused list
	*/

	Node* unusedHead = head;
	head = head->next;

	auto hangIntoUnusedList = [&unusedHead](Node*& unusedList, Node* node)->void {
		if (unusedList == nullptr) {
			unusedList = node;
			unusedList->pre = unusedList->next = nullptr;
		}
		else {
			unusedList->pre = unusedHead;
			unusedHead->pre = nullptr;
			unusedHead->next = unusedList;
			unusedList = unusedHead;
		}
	};

	if (unusedHead->evt->GetEventType() == Event::EventType::EVENT_MOUSE) {	
		hangIntoUnusedList(unusedMouseEventsList, unusedHead);
	}
	else if (unusedHead->evt->GetEventType() == Event::EventType::EVENT_KEYBOARD) {
		hangIntoUnusedList(unusedKeyEvenstList, unusedHead);
	}
	else {
		/**
		*	no reached
		*/
	}

	count--;
}

void EventsQueue::Push(const KeyEvent& evt)
{
	Node* newNode = nullptr;

	if (unusedKeyEvenstList != nullptr) {
		Node* unusedHead = unusedKeyEvenstList;
		unusedKeyEvenstList = unusedKeyEvenstList->next;

		newNode = unusedHead;
		*dynamic_cast<KeyEvent*>(newNode->evt) = dynamic_cast<const KeyEvent&>(evt);
	}
	else {
		KeyEvent* newEvt = nullptr;
		newEvt = new KeyEvent(evt);
		newNode = new Node;
		newNode->evt = newEvt;
	}

	if (head == nullptr) {
		newNode->pre = newNode->next = nullptr;
		head = tail = newNode;
	}
	else {
		newNode->pre = tail;
		newNode->next = nullptr;
		tail = newNode;
	}

	count++;
}

void EventsQueue::Push(const MouseEvent& evt)
{
	Node* newNode = nullptr;

	if (unusedMouseEventsList != nullptr) {
		Node* unusedHead = unusedMouseEventsList;
		unusedMouseEventsList = unusedMouseEventsList->next;

		newNode = unusedHead;
		*dynamic_cast<MouseEvent*>(newNode->evt) = dynamic_cast<const MouseEvent&>(evt);
	}
	else {
		MouseEvent* newEvt = nullptr;
		newEvt = new MouseEvent(evt);
		newNode = new Node;
		newNode->evt = newEvt;
	}

	if (head == nullptr) {
		newNode->pre = newNode->next = nullptr;
		head = tail = newNode;
	}
	else {
		newNode->pre = tail;
		newNode->next = nullptr;
		tail = newNode;
	}

	count++;
}

