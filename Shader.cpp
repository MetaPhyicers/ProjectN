#include "Shader.h"
#include "Renderer.h"
#include <fstream>
#include <sstream>

Graphic::Shader::Shader()
	:shaderID(0)
{
}

Graphic::Shader::~Shader()
{
	Graphic::GLDeleteProgram(shaderID);
}

void Graphic::Shader::Use()
{
	Graphic::GLUseProgram(shaderID);
}

int Graphic::Shader::CreateProgram(const char* vexShaderSrc, const char* fragShaderSrc, const char* geoShaderSrc)
{
	using namespace Graphic;

	if (!(vexShaderSrc && fragShaderSrc)) {
		throw std::invalid_argument("Exception: Graphic::Shader::CreateProgram(): Null shader source code!");
	}

	// compile vertex shader
	GLuint vertex = GLCreateShader(GL_VERTEX_SHADER);
	GLShaderSource(vertex, 1, &vexShaderSrc, nullptr);
	GLCompileShader(vertex);
	CheckErorrStatus(vertex, GL_VERTEX_SHADER);

	// compile fragment shader
	GLuint fragment = GLCreateShader(GL_FRAGMENT_SHADER);
	GLShaderSource(fragment, 1, &fragShaderSrc, nullptr);
	GLCompileShader(fragment);
	CheckErorrStatus(fragment, GL_FRAGMENT_SHADER);

	// compile geometry shader, if it has.
	GLuint geometryShader = 0;
	if (geoShaderSrc) {
		geometryShader = GLCreateShader(GL_GEOMETRY_SHADER);
		GLShaderSource(geometryShader, 1, &geoShaderSrc, nullptr);
		GLCompileShader(geometryShader);
		CheckErorrStatus(geometryShader, GL_GEOMETRY_SHADER);
	}

	// attach program
	shaderID = GLCreateProgram();
	GLAttachShader(shaderID, vertex);
	GLAttachShader(shaderID, fragment);
	if (geoShaderSrc) {
		GLAttachShader(shaderID, geometryShader);
	}
	GLLinkProgram(shaderID);
	// check for linking errors
	CheckErorrStatus(shaderID, GL_LINK_STATUS);

	GLDeleteShader(vertex);
	GLDeleteShader(fragment);
	if (geoShaderSrc) {
		GLDeleteShader(geometryShader);
	}

	return shaderID;
}

int Graphic::Shader::CreateProgramFromFile(const char* vertexShaderpath, const char* fragmentShaderPath, const char* geoShaderPath)
{
	if (!(vertexShaderpath && fragmentShaderPath)) {
		return 0;
	}
	std::ifstream shaderFile;
	std::stringstream vsStream, fsStream, gsStream;
	std::string vsCode, fsCode, gsCode;

	std::string throwMessage = "Exception: Graphic::Shader::CreateProgramFromFile(): Open file failed, Which is ";

	shaderFile.exceptions(std::ios_base::badbit | std::ios_base::failbit);

	try
	{
		shaderFile.open(vertexShaderpath, std::ios_base::in);
		vsStream << shaderFile.rdbuf();
		vsCode = vsStream.str();
		shaderFile.close();
	}
	catch (const std::exception& exp)
	{
		throwMessage.append(vertexShaderpath);
		throw std::invalid_argument(throwMessage.c_str());
	}

	try
	{
		shaderFile.open(fragmentShaderPath, std::ios_base::in);
		fsStream << shaderFile.rdbuf();
		fsCode = fsStream.str();
		shaderFile.close();
	}
	catch (const std::exception& exp)
	{
		throwMessage.append(fragmentShaderPath);
		throw std::invalid_argument(throwMessage.c_str());
	}

	if (geoShaderPath) {
		try
		{
			shaderFile.open(geoShaderPath, std::ios_base::in);
			gsStream << shaderFile.rdbuf();
			gsCode = gsStream.str();
			shaderFile.close();
		}
		catch (const std::exception& exp)
		{
			throwMessage.append(geoShaderPath);
			throw std::invalid_argument(throwMessage.c_str());
		}

		return CreateProgram(vsCode.c_str(), fsCode.c_str(), gsCode.c_str());
	}

	return CreateProgram(vsCode.c_str(), fsCode.c_str());	
}

void Graphic::Shader::SetBool(GLuint location, GLboolean value) const
{
	Graphic::GLUniform1i(location, value);
}

void Graphic::Shader::SetInt(GLuint location, GLint value) const
{
	Graphic::GLUniform1i(location, value);
}

void Graphic::Shader::SetFloat(GLuint location, GLfloat value) const
{
	Graphic::GLUniform1f(location, value);
}

void Graphic::Shader::SetVec2(GLuint location, const glm::vec2& value)
{
	Graphic::GLUniform2fv(location, 1, glm::value_ptr(value));
}

void Graphic::Shader::SetVec2(GLuint location, GLfloat x, GLfloat y)
{
	Graphic::GLUniform2f(location, x, y);
}

void Graphic::Shader::SetVec3(GLuint location, glm::vec3& value) const
{
	Graphic::GLUniform3fv(location, 1, glm::value_ptr(value));
}

void Graphic::Shader::SetVec3(GLuint location, GLfloat x, GLfloat y, GLfloat z) const
{
	Graphic::GLUniform3f(location, x, y, z);
}

void Graphic::Shader::SetVec4(GLuint location, const glm::vec4& value) const
{
	Graphic::GLUniform4fv(location, 1, glm::value_ptr(value));
}

void Graphic::Shader::SetVec4(GLuint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const
{
	Graphic::GLUniform4f(location, x, y, z, w);
}

void Graphic::Shader::SetMat3(GLuint location, glm::mat3& value) const
{
	GLint transformLoc = location;
	Graphic::GLUniformMatrix3fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void Graphic::Shader::SetMat4(GLuint location, glm::mat4& value) const
{
	GLint transformLoc = location;
	Graphic::GLUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

GLuint Graphic::Shader::GetLocation(const char* name)
{
	return Graphic::GLGetUniformLocation(shaderID, name);
}

void Graphic::Shader::CheckErorrStatus(GLuint program, GLenum type)
{
	int success;
	char infoLog[512];

	std::string throwMessage = "Exception: Graphic::Shader::CheckErrorStatus():";

	if (GL_VERTEX_SHADER == type || GL_FRAGMENT_SHADER == type || GL_GEOMETRY_SHADER == type) {
		Graphic::GLGetShaderiv(program, GL_COMPILE_STATUS, &success);
		if (!success) {
			Graphic::GLGetShaderInfoLog(program, 512, nullptr, infoLog);
			throwMessage.append(infoLog);
			throw std::runtime_error(throwMessage.c_str());
		}
	}
	else if (GL_LINK_STATUS == type) {
		Graphic::GLGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			Graphic::GLGetProgramInfoLog(program, 512, nullptr, infoLog);
			throwMessage.append(infoLog);
			throw std::runtime_error(throwMessage.c_str());
		}
	}
	else {
		throwMessage.append("Unregonized status or shader type!");
		throw std::invalid_argument(throwMessage.c_str());
	}
}
