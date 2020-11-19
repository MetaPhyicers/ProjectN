#pragma once
#include "Utility.h"
#include "Technique.h"

class FontTech : public Technique
{
public:
	FontTech();
	virtual ~FontTech();

	bool Init();

	void SetProjection(glm::mat4& projection);
	void SetColor(glm::vec4& color);
	void BindTexture(GLuint textureID);

private:
	GLuint projectionLocation;
	GLuint colorLocation;

};

