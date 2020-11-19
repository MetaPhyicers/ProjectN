#include "Renderer.h"
#include "Widgets.h"
#include "Shader.h"
#include <iostream>
#include <stdexcept>
#include <stdexcept>
#include <Windows.h>

// thread not safety
Graphic::Renderer* g_pRenderer = nullptr;

Graphic::Renderer::Renderer()
	:targetList(), updateCallBack(nullptr)
{
	g_pRenderer = this;
}

Graphic::Renderer::Renderer(const Renderer& renderer)
	:targetList(renderer.targetList), updateCallBack(renderer.updateCallBack)
{
}

Graphic::Renderer::~Renderer()
{
	// release all targets
	for (Graphic::RenderTarget* target : targetList) {
		SafeDelete(target);
	}
}

void Graphic::Renderer::AddObeject(RenderTarget* target)
{
	if (g_pRenderer == nullptr) {
		return;
	}
	if (target == nullptr) {
		throw std::runtime_error("Exception::Graphic::Renderer::AddObject(): Null render target!");
	}
	g_pRenderer->targetList.push_back(target);
}

void Graphic::Renderer::SetUpdateCallBack(UpdateCallBack updateFunc)
{
	if (g_pRenderer == nullptr) {
		return;
	}
	g_pRenderer->updateCallBack = updateFunc;
}

void Graphic::Renderer::Render(float dt)
{
	// calling update function
	try
	{
		if (updateCallBack) {
			updateCallBack(dt);
		}

		// render all targets
		for (Graphic::RenderTarget* target : targetList) {
			target->Render(dt);
		}
	}
	catch (const std::exception& excep)
	{
		throw excep;
	}
}

Graphic::Primitive::Primitive()
	:vertexArrayObject(0), vertexBufferObject(0), indexArrayObject(0), vertexCount(0), indexCount(0), storageType(UNKNOWN_TYPE)
{
}

Graphic::Primitive::Primitive(const Primitive& primitive)
{
	this->vertexArrayObject = primitive.vertexArrayObject;
	this->indexArrayObject = primitive.indexArrayObject;
	this->vertexBufferObject = primitive.vertexBufferObject;
	this->vertexCount = primitive.vertexCount;
	this->indexCount = primitive.indexCount;
}

Graphic::Primitive::~Primitive()
{
}

void Graphic::Primitive::CreateBuffer(GLenum target)
{
	if (vertexArrayObject == 0) {
		GLCall(glGenVertexArrays(1, &vertexArrayObject));
	}

	switch (target)
	{
	case GL_ARRAY_BUFFER:
		GLCall(glGenBuffers(1, &vertexBufferObject));
		break;

	case GL_ELEMENT_ARRAY_BUFFER:
		GLCall(glGenBuffers(1, &indexArrayObject));
		break;

	default:
		throw std::invalid_argument("Exception: Render::Primitive::CreateBuffer(): Invalid buffer target!");
		break;
	}

	return;
}

void Graphic::Primitive::AttachBuffer(GLenum target, size_t size, void* data, GLenum usage)
{
	if (vertexArrayObject == 0) {
		throw std::runtime_error("Exception: Render::Primitive::AttachBuffer(): No vertex array object has been created!");
	}
	// bind vertex array
	GLCall(glBindVertexArray(vertexArrayObject));

	// choose data target
	switch (target)
	{
	case GL_ARRAY_BUFFER:
		GLCall(glBindBuffer(target, vertexBufferObject));
		vertexCount = size / sizeof(GLfloat);
		break;
	
	case GL_ELEMENT_ARRAY_BUFFER:
		GLCall(glBindBuffer(target, indexArrayObject));
		indexCount = size / sizeof(GLuint);
		break;

	default:
		throw std::invalid_argument("Exception: Render::Primitive::CreateBuffer(): Invalid buffer target!");
		break;
	}

	// choose storage type
	switch (usage)
	{
	case GL_STATIC_DRAW:
		storageType = STATIC;
		break;
	
	case GL_DYNAMIC_DRAW:
		storageType = DYNAMIC;
		break;

	default:
		throw std::invalid_argument("Exception: Render::Primitive::CreateBuffer(): Invalid data usage!");
		break;
	}


	GLCall(glBufferData(target, size, data, usage));

	return;
}

void Graphic::Primitive::DetachBuffer()
{
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Graphic::Primitive::BufferSubData(GLenum target, size_t offset, size_t size, void* data)
{

	if (GL_ARRAY_BUFFER == target) {
		GLCall(glBindBuffer(target, vertexBufferObject));
	}
	else if (GL_ELEMENT_ARRAY_BUFFER == target) {
		GLCall(glBindBuffer(target, vertexBufferObject));
	}
	else {
		throw std::invalid_argument("Exception: Render::Primitive::CreateBuffer(): Invalid buffer target!");
	}
	GLCall(glBufferSubData(target, offset, size, data));
}

void Graphic::Primitive::AttribPointer(GLuint layout, size_t numberOfCompoments, size_t stride, const void* offsetPointer)
{
	GLCall(glEnableVertexAttribArray(layout));
	GLCall(glVertexAttribPointer(layout, numberOfCompoments, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), offsetPointer));
}


void Graphic::Primitive::Render(float dt)
{
	GLCall(glBindVertexArray(vertexArrayObject));

	// use index buffer
	if (indexArrayObject != 0) {
		GLCall(glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr));
	}
	else {
		GLCall(glDrawArrays(GL_TRIANGLES, 0, vertexCount));
	}
}

Graphic::Primitive::StorageType Graphic::Primitive::GetStorageType()
{
	return storageType;
}

void Graphic::GLEnable(GLenum capbility)
{
	GLCall(glEnable(capbility));
}

void Graphic::GLDisable(GLenum capbility)
{
	GLCall(glDisable(capbility));
}

void Graphic::GLGenTextures(GLsizei n, GLuint* texture)
{
	GLCall(glGenTextures(n, texture));
}

void Graphic::GLBindTexture(GLenum target, GLuint texture)
{
	GLCall(glBindTexture(target, texture));
}

void Graphic::GLTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)
{
	GLCall(glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels));
}

void Graphic::GLTexParameteri(GLenum target, GLenum pname, GLint param)
{
	GLCall(glTexParameteri(target, pname, param));
}


void Graphic::GLBlendFunc(GLenum sourceFactor, GLenum destinationFactor)
{
	GLCall(glBlendFunc(sourceFactor, destinationFactor));
}

GLuint Graphic::GLCreateShader(GLenum shaderType)
{
	return glCreateShader(shaderType);
}

void Graphic::GLShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length)
{
	GLCall(glShaderSource(shader, count, string, length));
}

void Graphic::GLCompileShader(GLuint shader)
{
	GLCall(glCompileShader(shader));
}

GLuint Graphic::GLCreateProgram()
{
	return glCreateProgram();
}

void Graphic::GLAttachShader(GLuint program, GLuint shader)
{
	GLCall(glAttachShader(program, shader));
}

void Graphic::GLLinkProgram(GLuint program)
{
	GLCall(glLinkProgram(program));
}

void Graphic::GLDeleteShader(GLuint shader)
{
	GLCall(glDeleteShader(shader));
}

void Graphic::GLDeleteProgram(GLuint program)
{
	GLCall(glDeleteProgram(program));
}

void Graphic::GLUseProgram(GLuint program)
{
	GLCall(glUseProgram(program));
}

void Graphic::GLGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
	GLCall(glGetShaderInfoLog(shader, maxLength, length, infoLog));
}

void Graphic::GLGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
	GLCall(glGetProgramInfoLog(program, maxLength, length, infoLog));
}

void Graphic::GLGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	GLCall(glGetShaderiv(shader, pname, params));
}

void Graphic::GLGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
	GLCall(glGetProgramiv(program, pname, params));
}

GLuint Graphic::GLGetUniformLocation(GLuint program, const GLchar* name)
{
	Debug::GLClearError();
	GLuint location = glGetUniformLocation(program, name);
	Debug::GLLogCall("gGenUniformLocation", __FILE__, __LINE__);
	return location;
}

void Graphic::GLUniform1i(GLint location, GLint v0)
{
	GLCall(glUniform1i(location, v0));
}

void Graphic::GLUniform1f(GLint location, GLfloat v0)
{
	GLCall(glUniform1f(location, v0));
}

void Graphic::GLUniform2f(GLuint location, GLfloat v0, GLfloat v1)
{
	GLCall(glUniform2f(location, v0, v1));
}

void Graphic::GLUniform2fv(GLuint location, GLsizei count, const GLfloat* value)
{
	GLCall(glUniform2fv(location, count, value));
}

void Graphic::GLUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
	GLCall(glUniform3f(location, v0, v1, v2));
}

void Graphic::GLUniform3fv(GLint location, GLsizei count, const GLfloat* value)
{
	GLCall(glUniform3fv(location, count, value));
}

void Graphic::GLUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	GLCall(glUniform4f(location, v0, v1, v2, v3));
}

void Graphic::GLUniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
	GLCall(glUniform4fv(location, count, value));
}

void Graphic::GLUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	GLCall(glUniformMatrix3fv(location, count, transpose, value));
}

void Graphic::GLUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	GLCall(glUniformMatrix4fv(location, count, transpose, value));
}

Graphic::RenderTarget::RenderTarget()
	:primitive(new Primitive)
{
}

Graphic::RenderTarget::RenderTarget(const RenderTarget& renderTarget)
{
	memcpy_s(this->primitive, sizeof(Graphic::Primitive), renderTarget.primitive, sizeof(Graphic::Primitive));
}

Graphic::RenderTarget::~RenderTarget()
{
	delete primitive;
}
