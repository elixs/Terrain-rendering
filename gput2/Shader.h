#pragma once

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Utils.h"

class GShader
{
public:
	GShader(const char* VertexPath, const char* FragmentPath);
	GShader(int VertexResource, int FragmentResource);

	void InitShader(const char* VertexPath, const char* FragmentPath);
	void Use();
	void SetBool(const char* Name, bool Value) const;
	void Set1i(const char* Name, int Value1) const;
	void Set1f(const char* Name, float Value1) const;
	void Set2f(const char* Name, float Value1, float Value2) const;
	void Set3f(const char* Name, float Value1, float Value2, float Value3) const;
	void Set4f(const char* Name, float Value1, float Value2, float Value3, float Value4) const;
	void Set3fv(const char* Name, float* Vector) const;
	void SetVec3(const char* Name, glm::vec3 Vector) const;
	void SetVec2r(const char* Name, glm::vec2 Rotation) const;
	void SetMatrix4fv(const char* Name, float* Value) const;
	void SetMat4(const char* Name, glm::mat4 Matrix) const;

public:
	unsigned int Id;
};

__forceinline GShader::GShader(const char* VertexPath, const char* FragmentPath)
{
	InitShader(FileToChar(VertexPath), FileToChar(FragmentPath));
}

__forceinline GShader::GShader(int VertexResource, int FragmentResource)
{
	HRSRC Vertex = FindResource(NULL, MAKEINTRESOURCE(VertexResource), "SHADER");
	HGLOBAL VertexData = LoadResource(NULL, Vertex);

	HRSRC Fragment = FindResource(NULL, MAKEINTRESOURCE(FragmentResource), "SHADER");
	HGLOBAL FragmentData = LoadResource(NULL, Fragment);

	InitShader((char*)LockResource(VertexData), (char*)LockResource(FragmentData));
}
__forceinline void GShader::InitShader(const char* VertexCode, const char* FragmentCode)
{
	unsigned int Vertex, Fragment;
	int Success;
	char InfoLog[512];

	// Vertex Shader
	Vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(Vertex, 1, &VertexCode, NULL);
	glCompileShader(Vertex);
	glGetShaderiv(Vertex, GL_COMPILE_STATUS, &Success);
	if (!Success)
	{
		glGetShaderInfoLog(Vertex, 512, NULL, InfoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << InfoLog << std::endl;
	};

	// Fragment Shader
	Fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(Fragment, 1, &FragmentCode, NULL);
	glCompileShader(Fragment);
	glGetShaderiv(Fragment, GL_COMPILE_STATUS, &Success);
	if (!Success)
	{
		glGetShaderInfoLog(Fragment, 512, NULL, InfoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << InfoLog << std::endl;
	};

	// Shader Program
	Id = glCreateProgram();
	glAttachShader(Id, Vertex);
	glAttachShader(Id, Fragment);
	glLinkProgram(Id);
	glGetProgramiv(Id, GL_LINK_STATUS, &Success);
	if (!Success)
	{
		glGetProgramInfoLog(Id, 512, NULL, InfoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << InfoLog << std::endl;
	}

	glDeleteShader(Vertex);
	glDeleteShader(Fragment);
}

__forceinline void GShader::Use()
{
	glUseProgram(Id);
}

__forceinline void GShader::SetBool(const char* Name, bool Value) const
{
	glUniform1i(glGetUniformLocation(Id, Name), (int)Value);
}

__forceinline void GShader::Set1i(const char* Name, int Value1) const
{
	glUniform1i(glGetUniformLocation(Id, Name), Value1);
}

__forceinline void GShader::Set1f(const char* Name, float Value1) const
{
	glUniform1f(glGetUniformLocation(Id, Name), Value1);
}

__forceinline void GShader::Set2f(const char* Name, float Value1, float Value2) const
{
	glUniform2f(glGetUniformLocation(Id, Name), Value1, Value2);
}

__forceinline void GShader::Set3f(const char* Name, float Value1, float Value2, float Value3) const
{
	glUniform3f(glGetUniformLocation(Id, Name), Value1, Value2, Value3);
}

__forceinline void GShader::Set4f(const char* Name, float Value1, float Value2, float Value3, float Value4) const
{
	glUniform4f(glGetUniformLocation(Id, Name), Value1, Value2, Value3, Value4);
}

__forceinline void GShader::Set3fv(const char* Name, float* Vector) const
{
	glUniform3fv(glGetUniformLocation(Id, Name), 1, &Vector[0]);
}

__forceinline void GShader::SetVec3(const char* Name, glm::vec3 Vector) const
{
	glUniform3fv(glGetUniformLocation(Id, Name), 1, &Vector[0]);
}

void GShader::SetVec2r(const char* Name, glm::vec2 Rotation) const
{
	glm::vec3 Normal(glm::cos(glm::radians(Rotation.y))*glm::cos(glm::radians(Rotation.x)),
		glm::sin(glm::radians(Rotation.y))*glm::cos(glm::radians(Rotation.x)),
		glm::sin(glm::radians(Rotation.x)));
	glUniform3fv(glGetUniformLocation(Id, Name), 1, &Normal[0]);
}

__forceinline void GShader::SetMatrix4fv(const char* Name, float* Value) const
{
	glUniformMatrix4fv(glGetUniformLocation(Id, Name), 1, GL_FALSE, Value);
}

__forceinline void GShader::SetMat4(const char* Name, glm::mat4 Matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(Id, Name), 1, GL_FALSE, &Matrix[0][0]);
}
