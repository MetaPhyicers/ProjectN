#pragma once
#include "Utility.h"
#include "Technique.h"

class MeshTech : public Technique
{
public:
	MeshTech();
	~MeshTech();

	bool Init();

	void SetProjectionView(glm::mat4& projection);
	void SetModel(glm::mat4& model);
	void SetDiffuseMap(GLuint textureUnit);
	void SetSpeculatMap(GLuint textureUnit);
	void SetAmbientMap(GLuint textureUnit);


	void BindTexture(GLuint textureID);
	void ActiveTexture(GLenum texture);

private:
	GLuint projectionLocation;
	GLuint viewLocation;
	GLuint modelLocation;

	struct TextureLocation
	{
		GLuint textureDiffuseLocation;
		GLuint textureSpecularLocation;
		GLuint textureAmbientLocation;
	};


	TextureLocation txLocation0;
	TextureLocation txLocation1;

};	
	
