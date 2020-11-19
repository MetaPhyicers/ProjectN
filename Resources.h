#pragma once
#include "Utility.h"

namespace Graphic
{
	class Shader;
}
namespace Widgets
{
	class Font;
}

class Resources
{
public:
	Resources();
	~Resources();

	static Graphic::Shader* CreateShader(const char* vsCode, const char* fsCode, const char* gsCode = nullptr);
	static Widgets::Font* CreateFontx(const wchar_t* path);

private:

	std::map<unsigned int, Graphic::Shader*> shaderSet;
	std::map<unsigned int, Widgets::Font*> fontSet;

};
