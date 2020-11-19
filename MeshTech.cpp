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
		gl_Position = projectionView * model * vec4(aPosition, 1.0);
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
	
	struct Texture
	{
		sampler2D textureDiffuse;
		sampler2D textureSpecular;
		sampler2D textureAmbient;
	};
	
	uniform Texture tx0;
	uniform Texture tx1;
	
	void main()
	{
		vec4 diffuse = texture(tx0.textureDiffuse, textureCoord) + texture(tx1.textureDiffuse, textureCoord);
		vec4 specular = texture(tx0.textureSpecular, textureCoord) + texture(tx1.textureSpecular, textureCoord);
		vec4 ambient = texture(tx0.textureAmbient, textureCoord) + texture(tx1.textureAmbient, textureCoord);
		
		FragColor = diffuse + specular + ambient;
	}
	)";

	shader = Resources::CreateShader(modelVSCode, modelFSCode);
	shader->Use();

	projectionLocation = shader->GetLocation("projectionView");
	modelLocation = shader->GetLocation("model");

	/** location 0 */
	tx0.textureDiffuseLocation = shader->GetLocation("tx0.textureDiffuse");
	tx0.textureSpecularLocation = shader->GetLocation("tx0.textureSpecular");
	tx0.textureAmbientLocation = shader->GetLocation("tx0.textureAmbient");
	/** location 1 */
	tx1.textureDiffuseLocation = shader->GetLocation("tx1.textureDiffuse");
	tx1.textureSpecularLocation = shader->GetLocation("tx1.textureSpecular");
	tx1.textureAmbientLocation = shader->GetLocation("tx1.textureAmbient");
	
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
	switch (textureUnit)
	{
	case 0:
		shader->SetInt(tx0.textureDiffuseLocation, textureUnit);
		break;
	case 1:
		shader->SetInt(tx1.textureDiffuseLocation, textureUnit);
		break;
	}
}

void MeshTech::SetSpeculatMap(GLuint textureUnit)
{
	switch (textureUnit)
	{
	case 0:
		shader->SetInt(tx0.textureSpecularLocation, textureUnit);
		break;
	case 1:
		shader->SetInt(tx1.textureSpecularLocation, textureUnit);
		break;
	}
}

void MeshTech::SetAmbientMap(GLuint textureUnit)
{
	switch (textureUnit)
	{
	case 0:
		shader->SetInt(tx1.textureAmbientLocation, textureUnit);
		break;
	case 1:
		shader->SetInt(tx1.textureAmbientLocation, textureUnit);
		break;
	}
}

void MeshTech::BindTexture(GLuint textureID)
{
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
}

void MeshTech::ActiveTexture(GLenum texture)
{
	GLCall(glActiveTexture(texture));
}

unsigned int MeshTech::GetID()
{
	return shader->GetID();
}
