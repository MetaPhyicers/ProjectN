#include "MeshTech.h"
#include "Resources.h"
#include "Debug.h"
#include "Shader.h"

MeshTech::MeshTech()
	:Technique()
{
}

MeshTech::~MeshTech()
{
}

bool MeshTech::Init()
{
	const char* modelVSCode = R"(
	#version 440
	#define MODEL_VERTEX_SHADER
	layout (location = 0) in vec3 aPosition;
	layout (location = 1) in vec3 aNormal;
	layout (location = 2) in vec2 aTextureCoord;
	layout (location = 3) in vec3 aTangent;
	layout (location = 4) in vec3 aBitangent;

	out vec3 normal;
	out vec2 textureCoord;	

	uniform mat4 projectionView;
	uniform mat4 model;

	void main()
	{
		gl_Position = model * projectionView * vec4(aPosition, 1.0);
		normal = aNormal;
		textureCoord = aTextureCoord;
	}
	
	)";

	const char* modelFSCode = R"(
	#version 440
	#define MODEL_FRAGMENT_SHADER

	in vec3 normal;
	in vec2 textureCoord;

	out vec4 FragColor;	

	uniform sampler2D textureDiffuse;
	uniform sampler2D textureSpecular;
	uniform sampler2D textureAmbient;
	
	void main()
	{
		vec4 diffuse = texture(textureDiffuse, textureCoord);
		vec4 specular = texture(textureSpecular, textureCoord);
		vec4 ambient = texture(textureAmbient, textureCoord);
		
		FragColor = diffuse + specular + ambient;
	}
	)";

	shader = Resources::CreateShader(modelVSCode, modelFSCode);
	shader->Use();

	projectionLocation = shader->GetLocation("projectionView");
	modelLocation = shader->GetLocation("model");
	textureDiffuseLocation = shader->GetLocation("textureDiffuse");
	textureSpecularLocation = shader->GetLocation("textureSpecular");
	textureAmbientLocation = shader->GetLocation("textureAmbient");
	
	glm::mat4 mat(1.f);
	shader->SetMat4(projectionLocation, mat);
	shader->SetMat4(modelLocation, mat);

	return true;
}

void MeshTech::SetProjectionView(glm::mat4& projection)
{
	shader->SetMat4(projectionLocation, projection);
}

void MeshTech::SetModel(glm::mat4& model)
{
	shader->SetMat4(modelLocation, model);
}

void MeshTech::SetDiffuseMap(GLuint textureUnit)
{
	shader->SetInt(textureDiffuseLocation, textureUnit);
}

void MeshTech::SetSpeculatMap(GLuint textureUnit)
{
	shader->SetInt(textureSpecularLocation, textureUnit);
}

void MeshTech::SetAmbientMap(GLuint textureUnit)
{
	shader->SetInt(textureAmbientLocation, textureUnit);
}

void MeshTech::BindTexture(GLuint textureID)
{
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
}

void MeshTech::ActiveTexture(GLenum texture)
{
	GLCall(glActiveTexture(texture));
}
