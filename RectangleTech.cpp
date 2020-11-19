#include "RectangleTech.h"
#include "Resources.h"
#include "Shader.h"
#include "Windows.h"
#include "Widgets.h"

RectangleTech::RectangleTech(RectStyle style)
	:Technique(), style(style)
{
}

RectangleTech::~RectangleTech()
{
}

bool RectangleTech::Init()
{
	// rectangle shader code

	const char* rectVSCode = R"(
	#version 440
	#define RECT_VERTEX_SHADER
	
	layout (location = 0) in vec4 vertex;
	
	out vec2 textureCoords;
	out vec2 position;
	
	uniform mat4 projection;
	uniform mat4 model;

	void main()
	{
		position = vertex.xy;
		textureCoords = vertex.zw;
		gl_Position = projection * model * vec4(position, 0.0, 1.0);
	}
	)";

	const char* rectFSCode = R"(
	#version 440
	#define RECT_FRAGMENT_SHADER
	
	in vec2 textureCoords;
	in vec2 position;
	
	out vec4 fragColor;
	
	uniform sampler2D text;
	uniform bool toggleTexture;
	uniform vec4 textColor;
	
	uniform vec2 center;
	uniform vec2 size;
	uniform float radius;

	
	bool InCircle(vec2 pos, vec2 center)
	{
		return (pos.x - center.x) * (pos.x - center.x) + (pos.y - center.y) * (pos.y - center.y) <= radius * radius;
	}
	
	bool CheckPixel(vec2 pos)
	{
		vec2 leftTop = vec2(center.x - size.x / 2 + radius, center.y + size.y / 2 - radius);
		vec2 rightTop = vec2(center.x + size.x / 2 - radius, center.y + size.y / 2 - radius);
		vec2 leftBottom = vec2(center.x - size.x / 2 + radius, center.y - size.y / 2 + radius);
		vec2 rightBottom = vec2(center.x + size.x / 2 - radius, center.y - size.y / 2 + radius);
	
		vec2 horiEdge = vec2(center.x - size.x / 2, center.x + size.x / 2);
		vec2 vertiEdge = vec2(center.y - size.y / 2, center.y + size.y / 2);

		// check pixel position and discard it which is beyound the boundary

		if(pos.x > horiEdge.x && pos.x < horiEdge.y && pos.y > vertiEdge.x && pos.y < vertiEdge.y){

			if(InCircle(pos, leftTop) || InCircle(pos, rightTop) || InCircle(pos, leftBottom) || InCircle(pos, rightBottom)){
				return true;
			}
			else if(pos.x > leftTop.x && pos.x < rightBottom.x && pos.y > center.y - size.y / 2 && pos.y < center.y + size.y / 2){
				return true;
			}else if(pos.x > center.x - size.x / 2 && pos.x < center.x + size.x / 2 && pos.y > rightBottom.y && pos.y < rightTop.y){
				return true;
			}

		}
	
		return false;
	}
	
	void main()
	{
		// If this pixel isn't in the region, discard it
		if(radius != 0.0 && !CheckPixel(position)){
			discard;
		}

		if(toggleTexture){
			vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, textureCoords).r);
			fragColor = textColor * sampled;
		}else{
			fragColor = textColor;
		}	

	}
	)";


	shader = Resources::CreateShader(rectVSCode, rectFSCode);
	shader->Use();

	/** get location */
	projectionLocation = shader->GetLocation("projection");
	modelLocation = shader->GetLocation("model");
	toggleTextureLocation = shader->GetLocation("toggleTexture");
	textColorLocation = shader->GetLocation("textColor");
	centerLocation = shader->GetLocation("center");
	radiusLocation = shader->GetLocation("radius");
	sizeLocation = shader->GetLocation("size");

	// initialize matrix
	glm::mat4 projection = glm::ortho(0.f, static_cast<float>(Window::GetWindowWidth()), 0.f, static_cast<float>(Window::GetWindowHeight()));
	glm::mat4 model(1.f);
	shader->SetMat4(projectionLocation, projection);
	shader->SetMat4(modelLocation, model);

	// initialize clolor, texture
	shader->SetBool(toggleTextureLocation, GL_FALSE);
	shader->SetVec4(textColorLocation, Graphic::DEFAULT_COLOR);

	// initialize radius, center position and size
	shader->SetFloat(radiusLocation, 0.f);
	shader->SetVec2(centerLocation, Widgets::DEFAULT_CENTER);
	shader->SetVec2(sizeLocation, Widgets::DEFAULT_SIZE);

	return true;
}

void RectangleTech::SetProjection(glm::mat4& projection)
{
	shader->SetMat4(projectionLocation, projection);
}

void RectangleTech::SetColor(glm::vec4& color)
{
	shader->SetVec4(textColorLocation, color);
}

void RectangleTech::ToggleTexture(bool status)
{
	shader->SetBool(toggleTextureLocation, status);
}

void RectangleTech::SetTexture(GLuint textureID)
{
	shader->SetBool(toggleTextureLocation, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void RectangleTech::SetCenter(glm::vec2& center)
{
	shader->SetVec2(centerLocation, center);
}

void RectangleTech::SetSize(glm::vec2& size)
{
	shader->SetVec2(sizeLocation, size);
}

void RectangleTech::SetRadius(float R)
{
	if (style == RECT_REGULAR) {
		shader->SetFloat(radiusLocation, 0.f);
	}
	else {
		shader->SetFloat(radiusLocation, R);
	}
}
