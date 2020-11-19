#include "Resources.h"
#include "Shader.h"
#include "Widgets.h"

Resources* g_pResourceManager;

Resources::Resources()
	:shaderSet(), fontSet()
{
	g_pResourceManager = this;
}

Resources::~Resources()
{
	for (auto& element : fontSet) {
		SafeDelete(element.second); 
	}

	for (auto& element : shaderSet) {
		SafeDelete(element.second); 
	}
}

Graphic::Shader* Resources::CreateShader(const char* vsCode, const char* fsCode, const char* gsCode)
{
	std::string code(vsCode);
	code.append(fsCode);
	
	// get hash code
	unsigned int hash = HashString::FNV_1A_Multibyte(code.c_str(), code.size());
	
	Graphic::Shader* shader = nullptr;
	try
	{
		shader = g_pResourceManager->shaderSet.at(hash);
	}
	catch (const std::exception&)
	{
		shader = new Graphic::Shader();
		shader->CreateProgram(vsCode, fsCode, gsCode);
		g_pResourceManager->shaderSet.insert(std::pair<unsigned int, Graphic::Shader*>(hash, shader));
	}

	return shader;
}

Widgets::Font* Resources::CreateFontx(const wchar_t* path)
{
	// get hash code
	unsigned int hash = HashString::FNV_1A_Unicode(path, wcsnlen_s(path, MAX_PATH));

	Widgets::Font* font = nullptr;
	try
	{
		font = g_pResourceManager->fontSet.at(hash);

	}
	catch (const std::exception&)
	{
		font = new Widgets::Font();
		font->LoadFont(path);
		g_pResourceManager->fontSet.insert(std::pair<unsigned int, Widgets::Font*>(hash, font));
	}

	return font;
}