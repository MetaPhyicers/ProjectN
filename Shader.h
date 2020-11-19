#pragma once
#include "Utility.h"

namespace Graphic
{
	class Shader
	{
	public:
		Shader();
		~Shader();

		void Use();
		int CreateProgram(const char* vexShaderSrc, const char* fragShaderSrc, const char* geoShaderSrc = nullptr);
		int CreateProgramFromFile(const char* vertexShaderpath, const char* fragmentShaderPath, const char* geoShaderPath = nullptr);

		void SetBool(GLuint location, GLboolean value) const;
		void SetInt(GLuint location, GLint value) const;
		void SetFloat(GLuint location, GLfloat value) const;

		void SetVec2(GLuint location, const glm::vec2& value);
		void SetVec2(GLuint location, GLfloat x, GLfloat y);

		void SetVec3(GLuint location, glm::vec3& value) const;
		void SetVec3(GLuint location, GLfloat x, GLfloat y, GLfloat z) const;

		void SetVec4(GLuint location, const glm::vec4& value) const;
		void SetVec4(GLuint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;

		void SetMat3(GLuint location, glm::mat3& value) const;
		void SetMat4(GLuint location, glm::mat4& value) const;

		GLuint GetLocation(const char* name);
		GLuint GetID() { return shaderID; }
	private:
		GLuint shaderID;
		void CheckErorrStatus(GLuint program, GLenum type);

	};

}

