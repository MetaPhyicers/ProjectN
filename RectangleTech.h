#pragma once
#include "Utility.h"
#include "Technique.h"

class RectangleTech : public Technique
{
public:
	enum RectStyle
	{
		RECT_SOFT,
		RECT_REGULAR
	};

	RectangleTech(RectStyle style = RECT_REGULAR);
	virtual ~RectangleTech();

	bool Init();

	void SetProjection(glm::mat4& projection);
	void SetColor(glm::vec4& color);
	void ToggleTexture(bool status);
	void SetTexture(GLuint textureID);
	void SetCenter(glm::vec2& center);
	void SetSize(glm::vec2& size);
	void SetRadius(float R);

private:
	RectStyle style;
	GLuint projectionLocation;
	GLuint modelLocation;
	GLuint toggleTextureLocation;
	GLuint textColorLocation;
	GLuint centerLocation;
	GLuint radiusLocation;
	GLuint sizeLocation;


	friend class Rect;
};

