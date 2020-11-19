#pragma once
#include "Debug.h"

class Window;

// render callback function
typedef void (*UpdateCallBack)(float dt);

namespace Graphic
{
	// state
	void GLEnable(GLenum capbility);
	void GLDisable(GLenum capbility);

	// texture
	void GLGenTextures(GLsizei n, GLuint* texture);
	void GLBindTexture(GLenum target, GLuint texture);
	void GLTexImage2D(GLenum target, GLint level, GLint internalformat,
		GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
	void GLTexParameteri(GLenum target, GLenum pname, GLint param);

	// blend
	void GLBlendFunc(GLenum sourceFactor, GLenum destinationFactor);

	// shader
	GLuint GLCreateShader(GLenum shaderType);
	GLuint GLCreateProgram();
	void GLShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
	void GLCompileShader(GLuint shader);
	void GLAttachShader(GLuint program, GLuint shader);
	void GLLinkProgram(GLuint program);
	void GLDeleteShader(GLuint shader);
	void GLDeleteProgram(GLuint program);
	void GLUseProgram(GLuint program);
	void GLGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
	void GLGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
	void GLGetShaderiv(GLuint shader, GLenum pname, GLint* params);
	void GLGetProgramiv(GLuint program, GLenum pname, GLint* params);

	// shader uniform
	GLuint GLGetUniformLocation(GLuint program, const GLchar* name);
	void GLUniform1i(GLint location, GLint v0);
	void GLUniform1f(GLint location, GLfloat v0);
	void GLUniform2f(GLuint location, GLfloat v0, GLfloat v1);
	void GLUniform2fv(GLuint location, GLsizei count, const GLfloat* value);
	void GLUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	void GLUniform3fv(GLint location, GLsizei count, const GLfloat* value);
	void GLUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void GLUniform4fv(GLint location, GLsizei count, const GLfloat* value);
	void GLUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	void GLUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

	class RenderTarget;
	class Renderer;
	class Primitive;
	class Shader;

	constexpr glm::vec4 DEFAULT_COLOR(1.0f);
}


/**
*	\description: class RenderTarget is the fundenmetal class of all rendering object
*/

class Graphic::RenderTarget
{
public:
	RenderTarget();
	RenderTarget(const RenderTarget& renderTarget);
	virtual ~RenderTarget();


protected:
	Graphic::Primitive* primitive;

	virtual bool Render(float dt) = 0;

	friend class Renderer;

};

/**
*	\description: class Renderer: manage render functions, only one Renderer in this program
*/

class Graphic::Renderer
{
public:
	Renderer();
	Renderer(const Renderer& renderer);
	~Renderer();

	static void AddObeject(RenderTarget* target);
	static void SetUpdateCallBack(UpdateCallBack updateFunc);
	void Render(float dt);

private: 
	std::list<RenderTarget*> targetList; ///< render taget list

	UpdateCallBack updateCallBack; ///< update callback function

};

/**
*	 \description: class Primitive: including vertices, color and texture, is used for canvas's concentrated rendering
*/

class Graphic::Primitive
{
public:
	Primitive();
	Primitive(const Primitive& primitive);

	~Primitive();

	void CreateBuffer(GLenum target);

	void AttachBuffer(GLenum target, size_t size, void* data, GLenum usage);
	void DetachBuffer();
	void BufferSubData(GLenum target, size_t offset, size_t size, void* data);
	void AttribPointer(GLuint layout, size_t numberOfCompoments, size_t stride, const void* offsetPointer);
	void Render(float dt);

public:
	enum StorageType
	{
		DYNAMIC,
		STATIC,
		UNKNOWN_TYPE
	};
	StorageType GetStorageType();

private:

	GLuint vertexBufferObject;
	GLuint vertexArrayObject;
	GLuint indexArrayObject;

	GLuint vertexCount;
	GLuint indexCount;

	StorageType storageType;
};