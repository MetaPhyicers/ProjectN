#include "FontTech.h"
#include "Resources.h"
#include "Shader.h"
#include "Windows.h"
#include "Widgets.h"

FontTech::FontTech()
	:Technique()
{
}

FontTech::~FontTech()
{
}

bool FontTech::Init()
{
	const char* fontVSCode = R"(
	#version 440 
	#define FONT_VERTEX_SHADER
	
	layout (location = 0) in vec4 vertex;
	
	out vec2 textureCoords;
	
	uniform mat4 projection;
	
	void main()
	{
	    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	    textureCoords = vertex.zw;
	}
	
	)";
	const char* fontFSCode = R"(
	#version 440
	#define FONT_FRAGEMENT_SHADER
	
	in vec2 textureCoords;
	
	out vec4 fragColor;
	
	uniform sampler2D text;
	uniform vec4 textColor;
	
	void main()
	{
		vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, textureCoords).r);
		fragColor = textColor * sampled;
	}
	
	)";

	shader = Resources::CreateShader(fontVSCode, fontFSCode);
	shader->Use();
	projectionLocation = shader->GetLocation("projection");
	colorLocation = shader->GetLocation("textColor");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Window::GetWindowWidth()), 0.0f, static_cast<float>(Window::GetWindowHeight()));
	shader->SetMat4(projectionLocation, projection);

	return false;
}

void FontTech::SetProjection(glm::mat4& projection)
{
	shader->SetMat4(projectionLocation, projection);
}

void FontTech::SetColor(glm::vec4& color)
{
	shader->SetVec4(colorLocation, color);
}

void FontTech::BindTexture(GLuint textureID)
{
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
}
