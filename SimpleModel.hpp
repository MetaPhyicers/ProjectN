#pragma once
#include "Technique.h"
#include "Shader.h"

class TriangleTech : public Technique
{
public:
	TriangleTech()
		:Technique()
	{

	}
	~TriangleTech()
	{

	}

	bool Init()
	{
		const char* vertexShader = R"(
			#version 440 core
			#define TRIANGLE_VERTEX_SHADER

			layout (location = 0) in vec3 pos;

			uniform mat4 m4ProjectionView;
			uniform mat4 m4Model;

			void main(){
				gl_Position = m4ProjectionView * m4Model * vec4(pos, 1.0);
			}
		)";
		const char* fragmentShader = R"(
			#version 440 core
			#define TRIANGLE_FRAGMENT_SHADER

			out vec4 fragColor;

			void main(){

				fragColor =  vec4(0.6f, 0.5f, 0.7f, 1.0f);
			}
		)";

		shader = Resources::CreateShader(vertexShader, fragmentShader);
		glm::mat4 projectionView(1.f);

		shader->Use();
		/**	get location */
		projectionLocation = shader->GetLocation("m4ProjectionView");
		modelLocation = shader->GetLocation("m4Model");

		shader->SetMat4(projectionLocation, projectionView);

		return true;
	}

	void SetProjection(glm::mat4& projectionView)
	{
		shader->Use();
		shader->SetMat4(projectionLocation, projectionView);
	}

	void SetModel(glm::mat4& model)
	{
		shader->Use();
		shader->SetMat4(modelLocation, model);
	}

private:
	GLuint projectionLocation;
	GLuint modelLocation;
};
class Triangle : public Graphic::RenderTarget
{
public:
	explicit Triangle(float centerX, float centerY, float centerZ, float edgeLength)
		:RenderTarget(), centerX(centerX), centerY(centerY), centerZ(centerZ),
		edgeLength(edgeLength), color(0.f, 0.f, 0.f, 1.f), technique(new TriangleTech())
	{
	}

	void Init()
	{
		double halfLength = static_cast<double>(edgeLength) / 2.0;
		double d = halfLength / sqrt(3.0);


		float vertices[] = {
			centerX - halfLength, centerY - d, centerZ,
			centerX + halfLength, centerY - d, centerZ,
			centerX, centerY + 2.0 * d, centerZ
		};

		primitive->CreateBuffer(GL_ARRAY_BUFFER);
		primitive->AttachBuffer(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
		primitive->AttribPointer(0, 3, 3, (void*)0);
		primitive->DetachBuffer();

		technique->Init();

	}

	void SetProjectionView(glm::mat4& projectionView)
	{
		technique->SetProjection(projectionView);
	}

	~Triangle()
	{
		SafeDelete(technique);
	}

	bool Render(float dt)
	{
		static glm::mat4 model = glm::mat4(1.f);
		model = glm::rotate(model, glm::radians(dt * 20.f), glm::vec3(1.f, 1.f, 0.f));

		technique->SetModel(model);

		primitive->Render(dt);

		return true;
	}

private:
	float centerX, centerY, centerZ;
	float edgeLength;

	glm::vec4 color;
	TriangleTech* technique;
};

class CubeTech : public Technique
{
public:
	CubeTech()
		:Technique()
	{
	}
	~CubeTech()
	{
	}

	bool Init()
	{
		const char* vertexShader = R"(
			#version 440 core
			#define CUBE_VERTEX_SHADER

			layout (location = 0) in vec3 av3Position;
			layout (location = 1) in vec2 av2TextureCoord;

			uniform mat4 m4ProjectionView;
			uniform mat4 m4Model;

			out vec2 v2TextureCoord;

			void main(){
				v2TextureCoord = av2TextureCoord;
				gl_Position = m4ProjectionView * m4Model * vec4(av3Position, 1.0);
			}
		)";
		const char* fragmentShader = R"(
			#version 440 core
			#define CUBE_FRAGMENT_SHADER

			uniform sampler2D sampler1;
			uniform vec4 v4Color;
			uniform bool toggleTexture;
			
			in vec2 v2TextureCoord;
			
			out vec4 fragColor;

			void main(){
				if(toggleTexture){
					fragColor = vec4(0.9f, 0.8f, 0.6f, 1.f) * texture(sampler1, v2TextureCoord);
				}else{				
					fragColor = vec4(0.9f, 0.8f, 0.6f, 1.f);
				}
			}
		)";

		shader = Resources::CreateShader(vertexShader, fragmentShader);
		glm::mat4 projectionView(1.f);

		shader->Use();
		/** get location */
		projectionLocation = shader->GetLocation("m4ProjectionView");
		toggleTextureLocation = shader->GetLocation("m4Model");
		toggleTextureLocation = shader->GetLocation("toggleTexture");

		shader->SetMat4(projectionLocation, projectionView);
		shader->SetBool(toggleTextureLocation, GL_FALSE);

		return true;
	}
	void SetProjection(glm::mat4& projectionView)
	{
		shader->Use();
		shader->SetMat4(projectionLocation, projectionView);
	}

	void SetModel(glm::mat4& model)
	{
		shader->Use();
		shader->SetMat4(modelLocation, model);
	}

private:
	GLuint projectionLocation;
	GLuint modelLocation;
	GLuint toggleTextureLocation;

};
class Cube : public Graphic::RenderTarget
{
public:
	explicit Cube(float centerX, float centerY, float centerZ, float edgeLength)
		:RenderTarget(), centerX(centerX), centerY(centerY), centerZ(centerZ), edgeLength(edgeLength),
		color(0.9f, 0.8f, 0.6f, 1.f), cubeTech(new CubeTech())
	{
	}
	~Cube()
	{
		SafeDelete(cubeTech);
	}

	void Init()
	{
		float halfLength = edgeLength / 2.f;

		float vertices[] = {
			// position, texture coordinate
			centerX - halfLength, centerY - halfLength, centerZ + halfLength, 0.f, 0.f,
			centerX + halfLength, centerY - halfLength, centerZ + halfLength, 1.f, 0.f,
			centerX + halfLength, centerY + halfLength, centerZ + halfLength, 1.f, 1.f,
			centerX - halfLength, centerY + halfLength, centerZ + halfLength, 0.f, 1.f,
			centerX - halfLength, centerY + halfLength, centerZ - halfLength, 1.f, 1.f,
			centerX + halfLength, centerY + halfLength, centerZ - halfLength, 0.f, 1.f,
			centerX + halfLength, centerY - halfLength, centerZ - halfLength, 0.f, 0.f,
			centerX - halfLength, centerY - halfLength, centerZ - halfLength, 1.f, 0.f
		};
		primitive->CreateBuffer(GL_ARRAY_BUFFER);
		primitive->AttachBuffer(GL_ARRAY_BUFFER, sizeof(float) * 40, vertices, GL_STATIC_DRAW);
		primitive->AttribPointer(0, 3, 5, (void*)0);
		primitive->AttribPointer(1, 2, 5, (void*)(sizeof(float) * 3));

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,
			7, 0, 3,
			3, 4, 7,
			6, 7, 4,
			4, 5, 6,
			1, 6, 5,
			5, 2, 1,
			3, 2, 5,
			5, 4, 3,
			7, 6, 1,
			1, 0, 7

		};
		primitive->CreateBuffer(GL_ELEMENT_ARRAY_BUFFER);
		primitive->AttachBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 36, indices, GL_STATIC_DRAW);
		primitive->DetachBuffer();

		cubeTech->Init();

	}


	void SetProjectionView(glm::mat4& projectionView)
	{	
		cubeTech->SetProjection(projectionView);
	}

	bool Render(float dt)
	{
		static glm::mat4 model(1.f);
		cubeTech->SetModel(model);

		primitive->Render(dt);
		return true;
	}
private:

	float centerX, centerY, centerZ;
	float edgeLength;

	glm::vec4 color;
	CubeTech* cubeTech;
};

class ADSLighting
{
public:
	ADSLighting(glm::vec4& ambientColor, glm::vec4& diffuseColor, glm::vec3& diffusePos, glm::vec4& specularColor,
		glm::vec3& specularPos)
	{

	}
	~ADSLighting()
	{

	}

private:
	glm::vec4 ambientColor;
	glm::vec4 diffuseColor;
	glm::vec3 diffusePos;
	glm::vec4 specularColor;
	glm::vec3 specularPos;

};