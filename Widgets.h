/**
*	\description: Graphic user interface components
*	\author: Darkming
*	\date: 2020-09-10
*	\lastEditTime: 2020-09-26
*	\lastEditer: Darkming
*
*	\detail: default color is RGBA(0.4f, 0.4f, 0.4f, 1.f)
*/
#pragma once


#include <ft2build.h>
#include <freetype/ftglyph.h>
#include FT_FREETYPE_H
#ifdef _DEBUG
#pragma comment(lib, "freetyped.lib")
#else
#pragma comment(lib, "freetype.lib")
#endif // _DEBUG

#include "Renderer.h"
#include "Utility.h"
#include "Event.h"

class Shader;
class ControlsManager;
class RectangleTech;
class FontTech;
class Graphic::Primitive;

namespace Widgets 
{
	class Rect;
	class Font;

	class BasicWidget;
	class StaticText;
	class Button;

	enum ControlStatus
	{
		CONTROL_ENABLE,
		CONTROL_VISIBLE,
		CONTROL_FOCUSED,

		CONTROL_UNKNOWN
	};


	constexpr glm::vec2 DEFAULT_POS(100.f, 100.f);
	constexpr glm::vec2 DEFAULT_SIZE(80.f, 80.f * 0.618f);
	constexpr glm::vec2 DEFAULT_CENTER(140.f, 100.f + 40.f * 0.618f);
};

/**
*	\brief: class BasicWidget: the fundanmetal class of all widget class
*/

class Widgets::BasicWidget : public Graphic::RenderTarget
{
public:
	BasicWidget();
	virtual ~BasicWidget();

	virtual bool Init() = 0;
	virtual bool Render(float dt) = 0;
	virtual bool Update(float dt) = 0;
	virtual bool Confirm(const Event& evt) = 0;
	
	void SetActionHandler(Event::EventAction action, Event::ActionHandler handler);

protected:

	struct Status
	{
		bool cursorDock;
		bool mouseLButton;
		bool mouseRButton;
	};

	static int count;
	uint32_t depth;
	Status status;
	ControlsManager* controlsManager;
	std::list<std::pair<Event::EventAction, Event::ActionHandler>> eventCallBacksTable;


	Event::ActionHandler* GetActionHandler(Event::EventAction action);
	void SetControlsManager(ControlsManager* controlsManager);
	const ControlsManager* GetControlsManager() const;

};

/**
*	\brief: class Rect: Rectangle
*/

class Widgets::Rect
{
public:
	enum RectStyle
	{
		RECT_SOFT,
		RECT_REGULAR
	};

	Rect(float left, float right, float top, float bottom, RectStyle style = RECT_REGULAR);
	Rect(const Rect& rect);
	~Rect();

	bool Init();
	bool Render(float dt);

	bool IsPointInside(float x, float y);
	void SetColor(glm::vec4& color);
	void SetColor(float r, float g, float b, float a);
	void GetSize(float& width, float& height);
	void GetLeftBottom(float& x, float& y);
	void GetRightTop(float& x, float& y);

	bool operator==(const Rect& rect) const;

private:
	float left;
	float right;
	float top;
	float bottom;

	glm::vec4 color;

	Graphic::Primitive* primitive;
	RectangleTech* rectTech;
	
};

/**
*	\brief: class Font: load font and render
*/

class Widgets::Font
{
public:
	Font();
	Font(const Font& font);
	~Font();

	void LoadFont(const wchar_t* fontPath);
	void AddCharacter(const wchar_t c);
	void Render2DText(glm::vec2& pos, glm::vec4& color, float scale, const std::wstring& text);
	void GetCharacterSize(const wchar_t ch, float scale, float & width, float & height, float & advance, float & bearingY);
	GLuint GetCharacterTexture(const wchar_t ch);

private:
	struct CharInfo
	{
		// character texture ID
		GLuint textureID;

		// character physical size
		GLuint advance;
		GLuint width;
		GLuint height;
		GLuint bearingX;
		GLuint bearingY;
		
	};

	// std::map use a binary tree to store data, with a LogN searching performance
	std::map<wchar_t, CharInfo> charSet;

	FT_Library ft;
	FT_Face face;

	bool isInitialzied;
	unsigned int id;

	Graphic::Primitive* primitive;
	FontTech* fontTech;
};

/**
*	\brief: class StaticText: rendering a text with a particular font
*/

class Widgets::StaticText : public Widgets::BasicWidget
{
public:
	enum TextStyle
	{
		TEXT_STYLE_NORMAL
	};
	enum TextPosition
	{
		TEXT_POS_RIGHT,
		TEXT_POS_LEFT,
		TEXT_POS_TOP,
		TEXT_POS_BOTTOM,
		TEXT_POS_LEFT_BOTTOM,
		TEXT_POS_RIGHT_BOTTOM,
		TEXT_POS_LEFT_TOP,
		TEXT_POS_RIGHT_TOP,
		TEXT_POS_VCENTER,
		TEXT_POS_HCENTER,
		TEXT_POS_VCENYER_HCENTER,
		TEXT_POS_MANUAL_ADJUST
	};

public:
	StaticText(const wchar_t* title, float x, float y, Widgets::StaticText::TextStyle style);
	StaticText(const StaticText& staticText);

	virtual ~StaticText();

	void SetFont(const wchar_t* fontPath);
	void SetColor(glm::vec4& color);
	void SetTitle(const std::wstring& text);
	void SetTextScale(float scale);
	void SetPosition(float x, float y);
	void SetPosition(TextPosition textPos);
	float GetTextScale();
	void GetTextOriginalSize(float& width, float& verticalMidPos, float& verticalMidToTopOffset, float& verticalMidToBottomOffset);
	void GetTextCurrentSize(float& width, float& verticalMidPos, float& verticalMidToTopOffset, float& verticalMidToBottomOffset);

	bool Init();

private:
	glm::vec2 pos;
	glm::vec2 textSize;
	glm::vec4 color;
	std::wstring title;
	Widgets::Font* font;
	float scale;

	TextStyle style;
	TextPosition posStatus;

	bool Update(float dt);
	bool Render(float dt);
	bool Confirm(const Event& evt);

	friend class ControlsManager;
	friend class Button;
};

/**
*	\brief: class Button
*/

class Widgets::Button : public Widgets::BasicWidget
{
public:
	enum ButtonStyle
	{
		BUTTON_CURVE,
		BUTTON_RECTANGLE
	};

public:
	Button(const wchar_t* title, float x, float y, float width, float height, ButtonStyle style);
	virtual ~Button();

	void SetTitle(const std::wstring& text);
	void SetPosition(float x, float y);

	bool Init();

protected:
	glm::vec2 pos;
	glm::vec2 size;
	glm::vec4 currentColor;
	glm::vec4 defaultColor;
	glm::vec4 clickedColor;
	glm::vec4 dockedColor;

	Widgets::Rect* rect;
	Widgets::StaticText* title;
	ButtonStyle style;

	bool Update(float dt);
	bool Render(float dt);
	bool Confirm(const Event& evt);

	void CalculateTextPosition();
	

	friend class ControlsManager;
};
