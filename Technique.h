#pragma once

namespace Graphic
{
	class Shader;
	class Renderer;
}


class Technique
{
public:
	Technique();
	virtual ~Technique();

	virtual bool Init() = 0;
	void Use();

protected:

	Graphic::Shader* shader;	
	
};
