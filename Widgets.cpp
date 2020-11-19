#include "Widgets.h"
#include "GUI.h"
#include "Windows.h"
#include "Shader.h"
#include "Renderer.h"

#include "RectangleTech.h"
#include "FontTech.h"
#include "Resources.h"

Widgets::Rect::Rect(float left, float right, float top, float bottom, RectStyle style)
	:left(left), right(right), top(top), bottom(bottom), color(0.4f, 0.4f, 0.4f, 1.f),
	primitive(new Graphic::Primitive()), rectTech(new RectangleTech((RectangleTech::RectStyle)style))
{
}

Widgets::Rect::Rect(const Rect& rect)
	:left(rect.left), right(rect.right), top(rect.top), bottom(rect.bottom), color(rect.color)
{
	///< deep copy class Primitive
	memcpy_s(this->primitive, sizeof(Graphic::Primitive), rect.primitive, sizeof(Graphic::Primitive));
}

Widgets::Rect::~Rect()
{
	///< release primitive and technique
	delete primitive;
	delete rectTech;
}

bool Widgets::Rect::Init()
{
	GLfloat rectangle[] = {
		// first triangle
		left, top, 0.f, 0.f,
		left, bottom, 0.f, 1.f,
		right, bottom, 1.f, 1.f,
		// secont triangle
		right, bottom, 0.f, 0.f,
		right, top, 1.f, 1.f,
		left, top, 1.f, 0.f
	};

	primitive->CreateBuffer(GL_ARRAY_BUFFER);
	primitive->AttachBuffer(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, rectangle, GL_STATIC_DRAW);
	primitive->AttribPointer(0, 4, 4, (void*)0);
	primitive->DetachBuffer();

	
	rectTech->Init();

	glm::mat4 projection = glm::ortho(0.f, static_cast<float>(Window::GetWindowWidth()), 0.f, static_cast<float>(Window::GetWindowHeight()));
	// set clolor, texture and projection
	rectTech->SetProjection(projection);
	rectTech->SetColor(color);
	rectTech->ToggleTexture(false);

	glm::vec2 center(left + (right - left) / 2.f, bottom + (top - bottom) / 2.f);
	glm::vec2 size(right - left, top - bottom);
	rectTech->SetCenter(center);
	rectTech->SetSize(size);	
	rectTech->SetRadius((top - bottom) * 0.1f);

	return true;
}

bool Widgets::Rect::IsPointInside(float x, float y)
{
	if (x >= left && x <= right) {
		if (y >= bottom && y <= top) {
			return true;
		}
	}

	return false;
}

void Widgets::Rect::SetColor(glm::vec4& color)
{
	this->color = color;
}

void Widgets::Rect::SetColor(float r, float g, float b, float a)
{
	this->color = glm::vec4(r, g, b, a);
}

void Widgets::Rect::GetSize(float& width, float& height)
{
	width = right - left;
	height = top - bottom;

	return;
}

void Widgets::Rect::GetLeftBottom(float& x, float& y)
{
	x = left;
	y = bottom;
}

void Widgets::Rect::GetRightTop(float& x, float& y)
{
	x = right;
	y = top;
}

bool Widgets::Rect::Render(float dt)
{
	/**
	*	apply updated status
	*/
	glm::vec2 center(left + (right - left) / 2.f, bottom + (top - bottom) / 2.f);
	glm::vec2 size(right - left, top - bottom);

	Graphic::GLDisable(GL_DEPTH_TEST);

	rectTech->Use();
	rectTech->SetCenter(center);
	rectTech->SetSize(size);
	rectTech->SetColor(color);
	rectTech->SetRadius((top - bottom) * 0.2f);
	primitive->Render(dt);

	Graphic::GLEnable(GL_DEPTH_TEST);

	return true;
}

bool Widgets::Rect::operator==(const Rect& rect) const
{
	if (this->left == rect.left && this->right == rect.right && this->top == rect.top && this->bottom == rect.bottom) {
		return true;
	}

	return false;
}

Widgets::Font::Font()
	:charSet(), ft(), face(), isInitialzied(false), id(0), primitive(new Graphic::Primitive()), fontTech(new FontTech())
{
}

Widgets::Font::Font(const Font& font)
	:ft(font.ft), face(font.face), isInitialzied(font.isInitialzied), id(0)
{
	///< deep copy class Primitive
	memcpy_s(this->primitive, sizeof(Graphic::Primitive), font.primitive, sizeof(Graphic::Primitive));
}

Widgets::Font::~Font()
{
	FT_Done_FreeType(ft);
	
	///< release primitve
	SafeDelete(primitive);
	SafeDelete(fontTech);
}


void Widgets::Font::LoadFont(const wchar_t* fontPath)
{
	fontTech->Init();
	
	primitive->CreateBuffer(GL_ARRAY_BUFFER);
	primitive->AttachBuffer(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	primitive->AttribPointer(0, 4, 4, 0);

	std::string path(Unicode::UnicodeToMultibytes(fontPath));
	if (FT_Init_FreeType(&ft)) {
		throw std::runtime_error("Exception: Widgets::Font::LoadFont(): Free type initialize failed!");
	}

	if (FT_New_Face(ft, path.c_str(), 0, &face)) {
		std::string throwMessage = "Exception: Widgets::FontLoadFont(): load " + path + " failed!";
		throw std::invalid_argument(throwMessage.c_str());
	}
	else {
		// The function sets the font's width and height parameters. 
		// Setting the width to 0 lets the face dynamically calculate the width based on the given height.
		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}
	// set unicode char map
	FT_Select_Charmap(face, ft_encoding_unicode);


	isInitialzied = true;
	id = HashString::FNV_1A_Unicode(fontPath, wcsnlen_s(fontPath, 260)); ///< 260 means MAX_PATH
}

void Widgets::Font::AddCharacter(const wchar_t c)
{
	using namespace Graphic;

	if (!isInitialzied) {
		throw std::invalid_argument("Exception: Widgets::Font::AddCharcter(): No font has loaded!");
	}

	// Load Unicode charactor
	FT_Glyph glyph;

	int error = 0;

	unsigned int index = FT_Get_Char_Index(face, static_cast<unsigned long>(c));

	error = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
	error = FT_Get_Glyph(face->glyph, &glyph);
	if (error) {
		throw std::invalid_argument("Exception: Widgets::Font::AddCharcter(): Failed to load glyph!");
	}
	else {
		error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
	}
	FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
	FT_Bitmap* bitmap = &bitmapGlyph->bitmap;


	GLuint texture;
	GLGenTextures(1, &texture);
	GLBindTexture(GL_TEXTURE_2D, texture);
	GLTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap->width, bitmap->rows, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap->buffer);

	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLBindTexture(GL_TEXTURE_2D, 0);

	CharInfo charInfo = {
		texture,
		face->glyph->advance.x,
		face->glyph->bitmap.width,
		face->glyph->bitmap.rows,
		face->glyph->bitmap_left,
		face->glyph->bitmap_top
	};
	charSet.insert(std::make_pair(c, charInfo));

	FT_Done_Glyph(glyph);	///< free glyph
}

void Widgets::Font::Render2DText(glm::vec2& pos, glm::vec4& color, float scale, const std::wstring& text)
{
	if (!isInitialzied) {
		throw std::invalid_argument("Exception: Widgets::Font::AddCharcter(): No font has been loaded!");
	}

	fontTech->Use();
	fontTech->SetColor(color);

	using namespace Graphic;
	// enable cull face
	GLEnable(GL_CULL_FACE);
	GLEnable(GL_BLEND);
	GLDisable(GL_DEPTH_TEST);
	GLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLfloat x = pos.x;
	GLfloat y = pos.y;

	for (wchar_t c : text) {
		// query whether the character is existed
		auto charInfo = charSet.find(c);
		if (charSet.end() == charInfo) {
			AddCharacter(c);
			charInfo = charSet.find(c);
		}

		GLfloat xPos = x + static_cast<GLfloat>(charInfo->second.bearingX) * scale;
		GLfloat yPos = y - static_cast<GLfloat>(charInfo->second.height - charInfo->second.bearingY) * scale;

		GLfloat w = static_cast<GLfloat>(charInfo->second.width) * scale;
		GLfloat h = static_cast<GLfloat>(charInfo->second.height) * scale;

		GLfloat vertices[6][4] = {
		{ xPos,     yPos + h,   0.0f, 0.0f },
		{ xPos,     yPos,       0.0f, 1.0f },
		{ xPos + w, yPos,       1.0f, 1.0f },

		{ xPos,     yPos + h,   0.0f, 0.0f },
		{ xPos + w, yPos,       1.0f, 1.0f },
		{ xPos + w, yPos + h,   1.0f, 0.0f }
		};

		fontTech->BindTexture(charInfo->second.textureID);

		primitive->BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		primitive->Render(0.f);

		// bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		x += (charInfo->second.advance >> 6) * scale;

	}

	// end of rendering
	GLDisable(GL_CULL_FACE);
	GLEnable(GL_DEPTH_TEST);
	GLBindTexture(GL_TEXTURE_2D, 0);

	primitive->DetachBuffer();
}

void Widgets::Font::GetCharacterSize(const wchar_t ch, float scale, float & width, float & height, float & advance, float & bearingY)
{
	/**
	*	try to find ch in charSet, otherwise load and append it into charSet
	*	we must use FLOAT for argument instead of SIZE_T, because static_cast<size_t>(float) will lost precision(it means lots of pixels).
	*/

	try
	{
		Font::CharInfo& exsitedChar = charSet.at(ch);
		width = static_cast<float>(exsitedChar.width) * scale;
		height = static_cast<float>(exsitedChar.height) * scale;
		advance = static_cast<float>(exsitedChar.advance >> 6) * scale;
		bearingY = static_cast<float>(exsitedChar.bearingY) * scale;
	}
	catch (const std::exception&)
	{
		AddCharacter(ch);
		Font::CharInfo& exsitedChar = charSet.at(ch);
		width = static_cast<float>(exsitedChar.width) * scale;
		height = static_cast<float>(exsitedChar.height) * scale;
		advance = static_cast<float>(exsitedChar.advance >> 6) * scale;
		bearingY = static_cast<float>(exsitedChar.bearingY) * scale;
	}

	return;
}

GLuint Widgets::Font::GetCharacterTexture(const wchar_t ch)
{
	try
	{
		CharInfo& exsitedChar = charSet.at(ch);
		return exsitedChar.textureID;
	}
	catch (const std::exception&)
	{
		AddCharacter(ch);
		CharInfo& exsitedChar = charSet.at(ch);
		return exsitedChar.textureID;
	}
	
	// no reached
	return 0;
}

Widgets::StaticText::StaticText(const wchar_t* title, float x, float y, Widgets::StaticText::TextStyle style)
	: Widgets::BasicWidget(), color(0.f, 0.f, 0.f, 1.f), title(title), pos(x, y), textSize(0, 0), scale(1.f),
	font(Resources::CreateFontx(L"C:\\windows\\Fonts\\msyh.ttc")), style(style)
{
}

Widgets::StaticText::StaticText(const StaticText& staticText)
	: BasicWidget(staticText), color(staticText.color), title(staticText.title), pos(staticText.pos), textSize(staticText.textSize),
	scale(staticText.scale), font(staticText.font)
{
}

Widgets::StaticText::~StaticText()
{
}

void Widgets::StaticText::SetFont(const wchar_t* fontPath)
{
	font = Resources::CreateFontx(fontPath);
	if (!font) {
		throw std::invalid_argument("Exception: Widgets::StaticText::SetFont(): invalid font path!");
	}
}

bool Widgets::StaticText::Init()
{
	return true;
}

bool Widgets::StaticText::Update(float dt)
{
	/**
	*	recalculate text position
	*/

	float width, verticalMidPos, verticalMidToTopOffset, verticalMidToBottomOffset;
	GetTextCurrentSize(width, verticalMidPos, verticalMidToTopOffset, verticalMidToBottomOffset);

	switch (posStatus)
	{
	case Widgets::StaticText::TEXT_POS_RIGHT:
		pos.x = Window::GetWindowWidth() - width;
		break;

	case Widgets::StaticText::TEXT_POS_LEFT:
		pos.x = 0.f;
		break;

	case Widgets::StaticText::TEXT_POS_TOP:
		pos.y = Window::GetWindowHeight() - (verticalMidToTopOffset + verticalMidToBottomOffset);
		break;

	case Widgets::StaticText::TEXT_POS_BOTTOM:
		pos.y = 0.f;
		break;

	case Widgets::StaticText::TEXT_POS_LEFT_BOTTOM:
		pos.x = 0.f;
		pos.y = 0.f;
		break;

	case Widgets::StaticText::TEXT_POS_RIGHT_BOTTOM:
		pos.x = Window::GetWindowWidth() - width;
		pos.y = 0.f;
		break;

	case Widgets::StaticText::TEXT_POS_LEFT_TOP:
		pos.x = 0.f;
		pos.y = Window::GetWindowHeight() - (verticalMidToTopOffset + verticalMidToBottomOffset);
		break;

	case Widgets::StaticText::TEXT_POS_RIGHT_TOP:
		pos.x = Window::GetWindowWidth() - width;
		pos.y = Window::GetWindowHeight() - (verticalMidToTopOffset + verticalMidToBottomOffset);
		break;

	case Widgets::StaticText::TEXT_POS_MANUAL_ADJUST:
		break;

	}

	return true;
}

bool Widgets::StaticText::Render(float dt)
{
	try
	{
		font->Render2DText(pos, color, scale, title);
	}
	catch (const std::exception& exp)
	{
		throw exp;
	}

	return true;
}

bool Widgets::StaticText::Confirm(const Event& evt)
{
	/**
	*	respose system events
	*/
	if (evt.GetEventType() == Event::EventType::EVENT_SYSTEM) {

	}

	return false;
}

void Widgets::StaticText::SetColor(glm::vec4& color)
{
	this->color = color;
}

void Widgets::StaticText::SetTitle(const std::wstring& text)
{
	this->title = text;
}

void Widgets::StaticText::SetTextScale(float scale)
{
	this->scale = scale;
}

void Widgets::StaticText::SetPosition(float x, float y)
{
	/**
	*	calling this function specified the text position is adjusting by manual
	*/
	pos = glm::vec2(x, y);
	posStatus = TEXT_POS_MANUAL_ADJUST;
}

void Widgets::StaticText::SetPosition(TextPosition textPos)
{
	posStatus = textPos;
}

float Widgets::StaticText::GetTextScale()
{
	return scale;
}

void Widgets::StaticText::GetTextOriginalSize(float& width, float& verticalMidPos, float& verticalMidToTopOffset, float& verticalMidToBottomOffset)
{
	width = 0.f;
	verticalMidPos = 0.f;
	verticalMidToTopOffset = 0.f;
	verticalMidToBottomOffset = 0.f;

	float maxYToTopOffset = 0;
	float maxYToBottomOffset = 0;

	for (wchar_t ch : title) {
		float charWidth, charHeight, charBearingY, charAdvance;

		/**
		*	if scale equals 1.0, return default size
		*/
		font->GetCharacterSize(ch, 1.f, charWidth, charHeight, charAdvance, charBearingY);
		maxYToTopOffset = charBearingY > maxYToTopOffset && charHeight - charBearingY == 0 ? charBearingY : maxYToTopOffset;
		maxYToBottomOffset = charBearingY - charHeight < 0 ? charHeight - charBearingY : maxYToBottomOffset;

		width += charAdvance;
	}

	verticalMidPos = pos.y;
	verticalMidToTopOffset = maxYToTopOffset;
	verticalMidToBottomOffset = maxYToBottomOffset;
}

void Widgets::StaticText::GetTextCurrentSize(float& width, float& verticalMidPos, float& verticalMidToTopOffset, float& verticalMidToBottomOffset)
{
	width = 0.f;
	verticalMidPos = 0.f;
	verticalMidToTopOffset = 0.f;
	verticalMidToBottomOffset = 0.f;

	float maxYToTopOffset = 0;
	float maxYToBottomOffset = 0;

	for (wchar_t ch : title) {
		float charWidth, charHeight, charBearingY, charAdvance;

		font->GetCharacterSize(ch, scale, charWidth, charHeight, charAdvance, charBearingY);
		maxYToTopOffset = charBearingY > maxYToTopOffset && charHeight - charBearingY == 0 ? charBearingY : maxYToTopOffset;
		maxYToBottomOffset = charBearingY - charHeight < 0 ? charHeight - charBearingY : maxYToBottomOffset;

		width += charAdvance;
	}

	verticalMidPos = pos.y;
	verticalMidToTopOffset = maxYToTopOffset;
	verticalMidToBottomOffset = maxYToBottomOffset;
}

int Widgets::BasicWidget::count = 0;

Widgets::BasicWidget::BasicWidget()
	:depth(count++), status(), controlsManager(nullptr), eventCallBacksTable()
{
}

Widgets::BasicWidget::~BasicWidget()
{

}

void Widgets::BasicWidget::SetActionHandler(Event::EventAction action, Event::ActionHandler handler)
{
	Event::ActionHandler* handle = GetActionHandler(action);
	if (handle == nullptr) {
		eventCallBacksTable.push_back(std::pair<Event::EventAction, Event::ActionHandler>(action, handler));
	}
	else {
		*handle = handler;
	}
}

Event::ActionHandler* Widgets::BasicWidget::GetActionHandler(Event::EventAction action)
{
	if (eventCallBacksTable.empty()) {
		return nullptr;
	}
	/**
	*	linear search
	*/
	for (std::list<std::pair<Event::EventAction, Event::ActionHandler>>::iterator p = eventCallBacksTable.begin();
		p != eventCallBacksTable.end(); ++p) {
		if (p->first == action && p->second) {
			return &(p->second);
		}
	}

	return nullptr;
}

void Widgets::BasicWidget::SetControlsManager(ControlsManager* controlsManager)
{
	this->controlsManager = controlsManager;
}

const ControlsManager* Widgets::BasicWidget::GetControlsManager() const
{
	return controlsManager;
}

Widgets::Button::Button(const wchar_t* title,float x, float y, float width, float height, Widgets::Button::ButtonStyle style)
	:BasicWidget(), pos(x, y), size(width, height), currentColor(0.4f, 0.5f, 0.5f, 0.8f), defaultColor(currentColor), 
	clickedColor(0.4f, 0.5f, 1.0f, 0.8f), dockedColor(0.4f, 0.5f, 0.8f, 0.8f), rect(nullptr), title(nullptr), style(style)
{
	this->title = new Widgets::StaticText(title, 0.f, 0.f, Widgets::StaticText::TEXT_STYLE_NORMAL);
	Rect::RectStyle rectStyle = Rect::RECT_REGULAR;
	switch (style)
	{
	case Widgets::Button::BUTTON_CURVE:
		rectStyle = Rect::RECT_SOFT;
		break;
	case Widgets::Button::BUTTON_RECTANGLE:
		rectStyle = Rect::RECT_REGULAR;
		break;
	}
	this->rect = new Rect(pos.x, pos.x + size.x, pos.y + size.y, pos.y, rectStyle);
}

Widgets::Button::~Button()
{
	SafeDelete(title);
	SafeDelete(rect);
}

void Widgets::Button::SetTitle(const std::wstring& text)
{
	title->SetTitle(text);
	/**
	*	recalculate text position
	*/
	CalculateTextPosition();
}

void Widgets::Button::SetPosition(float x, float y)
{
	pos.x = x;
	pos.y = y;

	/**
	*	modify rectagle position...
	*/

	/**
	*	recalculate text position
	*/
	CalculateTextPosition();
}

bool Widgets::Button::Init()
{
	rect->Init();
	CalculateTextPosition();

	return true;
}

bool Widgets::Button::Update(float dt)
{
	const glm::vec4 dColor(0.f, 0.f, 0.01f, 0.f);

	/**
	*	To judge if it has overlapped with other control, we polling controls manager
	*/
	if (controlsManager->GetCursorDockedContol() != this) {
		status.cursorDock = false;
	}

	if (status.cursorDock) {
		if (status.mouseLButton) {
			currentColor = clickedColor;
		}else if (currentColor.y < dockedColor.y) {
			currentColor += dColor;
		}
		else {
			currentColor = dockedColor;
		}
	}
	else {
		currentColor = defaultColor;
	}

	return true;
}

bool Widgets::Button::Render(float dt)
{	
	rect->SetColor(currentColor); ///< rendering rectangle
	rect->Render(dt);

	title->Render(dt);	///< rendering title

	return true;
}

bool Widgets::Button::Confirm(const Event& evt)
{
	/**
	*	process mouse event(s)
	*/
	if (evt.GetEventType() == Event::EventType::EVENT_MOUSE) {
		const MouseEvent& mouseEvt = dynamic_cast<const MouseEvent&>(evt);
		/**
		*	if cursor is inside the button
		*/
		if (this->rect->IsPointInside(static_cast<float>(mouseEvt.GetXPos()), static_cast<float>(mouseEvt.GetYPos()))) {

			/**
			*	set cursor dock status
			*/
			status.cursorDock = true;
			controlsManager->SetCursorDockedControl(this);

			Event::EventAction action = evt.GetEventAction();
			/**
			*	switch action
			*/
			switch (action)
			{
			case Event::EventAction::ACTION_LBUTTON_DOWN:
				status.mouseLButton = true;
				break;
			case Event::EventAction::ACTION_LBUTTON_RELEASE:
				status.mouseLButton = false;
				break;
			}

			/**
			*	find correspounding handler
			*/
			Event::ActionHandler* handle = GetActionHandler(action);
			if (handle != nullptr) {
				(*handle)(this, evt);
			}
					
			return true;
		}
		else {
			status.cursorDock = false;
		}
	}

	/**
	*	process key event(s), if it has focused
	*/
	else if (evt.GetEventType() == Event::EventType::EVENT_KEYBOARD) {
		const KeyEvent& keyEvt = dynamic_cast<const KeyEvent&>(evt);

		// keyEvt.GetKeyCode();

		Event::ActionHandler* handle = GetActionHandler(evt.GetEventAction());
		if (handle != nullptr) {
			(*handle)(this, evt);
		}
		return true;
	}

	return false;
}

void Widgets::Button::CalculateTextPosition()
{
	/**
	*	get original text size
	*/
	float width, verticalMidPos, verticalMidToTopOffset, verticalMidToBottomOffset;
	this->title->GetTextOriginalSize(width, verticalMidPos, verticalMidToTopOffset, verticalMidToBottomOffset);	

	/**
	*	adjust text size against rectangle
	*/
	float gap = width * 0.05f; ///< left 5% length of text for gap to the boundary
	float rectWidth, rectHeight, left, bottom;
	rect->GetSize(rectWidth, rectHeight);
	rect->GetLeftBottom(left, bottom);

	/**
	*	choose smaller one to modify text scale
	*/
	float scale = 1.f;
	if (rectHeight < (verticalMidToTopOffset + verticalMidToBottomOffset)) {
		scale = rectWidth / (width + 2 * gap);
	}

	
	/**
	*	recalculate text pos
	*/
	title->SetTextScale(scale);
	gap *= scale;
	title->GetTextCurrentSize(width, verticalMidPos, verticalMidToTopOffset, verticalMidToBottomOffset);
	title->SetPosition(left + gap + (rectWidth - width) / 2.f, bottom - (verticalMidToBottomOffset + verticalMidToTopOffset) * 0.5f + rectHeight * 0.5f);

}

