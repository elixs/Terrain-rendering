#pragma once

#define PI 3.14159265358979323846

#include <cmath>

#include <iostream>
#include <vector>

#include "stb_image.h"

char* FileToChar(const char *FilePath)
{
	FILE *File;
	long Length;
	char *Buffer;

	fopen_s(&File, FilePath, "rb"); /* Open file for reading */
	if (!File) /* Display error on failure */
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	fseek(File, 0, SEEK_END); /* Seek to the end of the file */
	Length = ftell(File); /* Find out how many bytes into the file we are */
	Buffer = (char*)malloc(Length + 1); /* Allocate a buffer for the entire length of the file and a null terminator */
	fseek(File, 0, SEEK_SET); /* Go back to the beginning of the file */
	fread(Buffer, Length, 1, File); /* Read the contents of the file in to the buffer */
	fclose(File); /* Close the file */
	Buffer[Length] = 0; /* Null terminator */

	return Buffer;
}

unsigned int LoadTexture(const char* TexturePath)
{
	unsigned int Texture;
	glGenTextures(1, &Texture);

	int Width, Height, Channels;

	stbi_set_flip_vertically_on_load(true);

	unsigned char *Data = stbi_load(TexturePath, &Width, &Height, &Channels, 0);
	if (Data)
	{
		GLenum Format;
		if (Channels == 1)
		{
			Format = GL_RED;
		}
		else if (Channels == 2)
		{
			Format = GL_RG;
		}
		else if (Channels == 3)
		{
			Format = GL_RGB;
		}
		else if (Channels == 4)
		{
			Format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(Data);

	return Texture;
}

glm::vec3 GetNormal(glm::vec2 Rotation)
{
	glm::vec3 Normal(glm::cos(glm::radians(Rotation.y))*glm::cos(glm::radians(Rotation.x)),
		glm::sin(glm::radians(Rotation.y))*glm::cos(glm::radians(Rotation.x)),
		glm::sin(glm::radians(Rotation.x)));
	return Normal;
}

void InsertVertex(float* VertexArray, int Index, float X, float Y, float Z, float NormalX, float NormalY, float NormalZ)
{
	VertexArray[Index] = X;
	VertexArray[Index + 1] = Y;
	VertexArray[Index + 2] = Z;
	VertexArray[Index + 3] = NormalX;
	VertexArray[Index + 4] = NormalY;
	VertexArray[Index + 5] = NormalZ;
}

void InsertVertex(float* VertexArray, int Index, float X, float Y, float Z, glm::vec3 Normal)
{
	InsertVertex(VertexArray, Index, X, Y, Z, Normal.x, Normal.y, Normal.z);
}

void InsertVertex(float* VertexArray, int Index, glm::vec3 Vertex, glm::vec3 Normal)
{
	InsertVertex(VertexArray, Index, Vertex.x, Vertex.y, Vertex.z, Normal.x, Normal.y, Normal.z);
}

void InsertVertex2D(float* VertexArray, int Index, float X, float Y)
{
	VertexArray[Index] = X;
	VertexArray[Index + 1] = Y;
}

void InsertVertex2D(float* VertexArray, int Index, glm::vec2 Vertex)
{
	VertexArray[Index] = Vertex.x;
	VertexArray[Index + 1] = Vertex.y;
}

void InsertIndex(int* IndexArray, int Index, int X, int Y, int Z)
{
	IndexArray[Index] = X;
	IndexArray[Index + 1] = Y;
	IndexArray[Index + 2] = Z;
}

void GenerateCylinder(float* &Cylinder, int* &CylinderIndices, int Vertices, float Radius, float Depth, int& CylinderSize, int& CylinderIndicesSize, bool bFlat = true)
{
	CylinderSize = 12 * (Vertices + 1);
	Cylinder = new float[CylinderSize];
	float* RCTheta = new float[Vertices];
	float* RSTheta = new float[Vertices];
	float HalfDepth = Depth / 2.f;

	for (int i = 0; i < Vertices; ++i)
	{
		float Theta = ((float)i / (float)Vertices) * 2.f * (float)PI;
		RCTheta[i] = Radius * std::cos(Theta);
		RSTheta[i] = Radius * std::sin(Theta);
	}

	InsertVertex(Cylinder, 0, 0.f, HalfDepth, 0.f, 0.f, 1.f, 0.f);
	InsertVertex(Cylinder, 6, 0.f, -HalfDepth, 0.f, 0.f, -1.f, 0.f);

	float FlatFactor = 1.f;
	if (bFlat)
	{
		FlatFactor = 0.f;
	}

	for (int i = 0; i < Vertices; ++i)
	{
		glm::vec3 NormalBottom = glm::normalize(glm::vec3(RCTheta[i], -1.f * FlatFactor, RSTheta[i]));
		glm::vec3 NormalTop = glm::normalize(glm::vec3(RCTheta[i], 1.f * FlatFactor, RSTheta[i]));

		InsertVertex(Cylinder, (i + 1) * 12, RCTheta[i], HalfDepth, RSTheta[i], NormalTop);
		InsertVertex(Cylinder, (i + 1) * 12 + 6, RCTheta[i], -HalfDepth, RSTheta[i], NormalBottom);
	}

	delete[] RCTheta;
	delete[] RSTheta;

	CylinderIndicesSize = 12 * Vertices;
	CylinderIndices = new int[CylinderIndicesSize];

	// Top
	for (int i = 0; i < Vertices; ++i)
	{
		InsertIndex(CylinderIndices, i * 3, 0, (i + 1) * 2, ((i + 1) % Vertices + 1) * 2);
	}

	// Side
	for (int i = 0; i < Vertices; ++i)
	{
		InsertIndex(CylinderIndices, (Vertices + i) * 3, (i + 1) * 2, (i + 1) * 2 + 1, ((i + 1) % Vertices) * 2 + 2);
	}

	for (int i = 0; i < Vertices; ++i)
	{
		InsertIndex(CylinderIndices, (2 * Vertices + i) * 3, (i + 1) * 2 + 1, ((i + 1) % Vertices) * 2 + 2, ((i + 1) % Vertices + 1) * 2 + 1);
	}

	// Bottom
	for (int i = 0; i < Vertices; ++i)
	{
		InsertIndex(CylinderIndices, (3 * Vertices + i) * 3, 1, (i + 1) * 2 + 1, ((i + 1) % Vertices + 1) * 2 + 1);
	}
}

void GenerateCone(float* &Cone, int* &ConeIndices, int Vertices, float Radius, float Legth, int& ConeSize, int& ConeIndicesSize, bool bFlat = true)
{
	ConeSize = 6 * (Vertices + 2);
	Cone = new float[ConeSize];
	float* RCTheta = new float[Vertices];
	float* RSTheta = new float[Vertices];

	for (int i = 0; i < Vertices; ++i)
	{
		float Theta = ((float)i / (float)Vertices) * 2.f * (float)PI;
		RCTheta[i] = Radius * std::cos(Theta);
		RSTheta[i] = Radius * std::sin(Theta);
	}

	glm::vec3 TopVertex(0.f, Legth, 0.f);

	InsertVertex(Cone, 0, 0.f, Legth, 0.f, 0.f, 1.f, 0.f);
	InsertVertex(Cone, 6, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f);

	float FlatFactor = 1.f;
	if (bFlat)
	{
		FlatFactor = 0.f;
	}

	for (int i = 0; i < Vertices; ++i)
	{
		glm::vec3 Vertex(RCTheta[i], 0.f, RSTheta[i]);
		glm::vec3 PreviousVertex(RCTheta[(Vertices + i - 1) % Vertices], 0.f, RSTheta[(Vertices + i - 1) % Vertices]);
		glm::vec3 NextVertex(RCTheta[(i + 1) % Vertices], 0.f, RSTheta[(i + 1) % Vertices]);

		glm::vec3 Normal = glm::normalize(glm::cross(PreviousVertex - Vertex, TopVertex - Vertex) + glm::cross(TopVertex - Vertex, NextVertex - Vertex) + glm::vec3(0.f, -2.f * FlatFactor, 0.f));

		InsertVertex(Cone, (i + 2) * 6, Vertex, Normal);
	}

	delete[] RCTheta;
	delete[] RSTheta;

	ConeIndicesSize = 6 * Vertices;
	ConeIndices = new int[ConeIndicesSize];

	for (int i = 0; i < Vertices; ++i)
	{
		InsertIndex(ConeIndices, i * 6, 0, i + 2, (i + 1) % Vertices + 2);
		InsertIndex(ConeIndices, i * 6 + 3, 1, (i + 1) % Vertices + 2, i + 2);
	}
}

void GenerateSphere(float* &Sphere, int* &SphereIndices, int Segments, int Rings, float Radius, int& SphereSize, int& SphereIndicesSize)
{
	SphereSize = 6 * (2 + (Rings - 1) * Segments);
	Sphere = new float[SphereSize];
	float** RCTSP = new float*[Rings - 1];
	for (int i = 0; i < Rings - 1; ++i)
	{
		RCTSP[i] = new float[Segments];
	}
	float** RSTSP = new float*[Rings - 1];
	for (int i = 0; i < Rings - 1; ++i)
	{
		RSTSP[i] = new float[Segments];
	}
	float** RCP = new float*[Rings - 1];
	for (int i = 0; i < Rings - 1; ++i)
	{
		RCP[i] = new float[Segments];
	}

	for (int i = 0; i < Rings - 1; ++i)
	{
		float Phi = (float)(i + 1) * (float)PI / (float)Rings;
		for (int j = 0; j < Segments; ++j)
		{
			float Theta = ((float)j / (float)Segments) * 2.f * (float)PI;
			float ST = std::sin(Theta);
			float CT = std::cos(Theta);
			float SP = std::sin(Phi);
			float CP = std::cos(Phi);
			RCTSP[i][j] = Radius * CT * SP;
			RSTSP[i][j] = Radius * ST * SP;
			RCP[i][j] = Radius * CP;
		}
	}

	glm::vec3 TopVertex(0.f, Radius, 0.f);
	glm::vec3 BottomVertex(0.f, -Radius, 0.f);

	InsertVertex(Sphere, 0, 0.f, Radius, 0.f, 0.f, 1.f, 0.f);
	InsertVertex(Sphere, 6, 0.f, -Radius, 0.f, 0.f, -1.f, 0.f);

	for (int i = 0; i < Rings - 1; ++i)
	{
		for (int j = 0; j < Segments; ++j)
		{
			glm::vec3 Vertex(RCTSP[i][j], RCP[i][j], RSTSP[i][j]);
			glm::vec3 LeftVertex(RCTSP[i][(Segments + j - 1) % Segments], RCP[i][(Segments + j - 1) % Segments], RSTSP[i][(Segments + j - 1) % Segments]);
			glm::vec3 RightVertex(RCTSP[i][(j + 1) % Segments], RCP[i][(j + 1) % Segments], RSTSP[i][(j + 1) % Segments]);
			glm::vec3 UpVertex;
			if (i == 0)
			{
				UpVertex = TopVertex;
			}
			else
			{
				UpVertex = glm::vec3(RCTSP[(Rings - 1 + i - 1) % (Rings - 1)][j], RCP[(Rings - 1 + i - 1) % (Rings - 1)][j], RSTSP[(Rings - 1 + i - 1) % (Rings - 1)][j]);
			}

			glm::vec3 DownVertex;
			if (i == Rings - 2)
			{
				DownVertex = BottomVertex;
			}
			else
			{
				DownVertex = glm::vec3(RCTSP[(i + 1) % (Rings - 1)][j], RCP[(i + 1) % (Rings - 1)][j], RSTSP[(i + 1) % (Rings - 1)][j]);
			}
			glm::vec3 Normal = glm::normalize(glm::cross(LeftVertex - Vertex, UpVertex - Vertex) + glm::cross(UpVertex - Vertex, RightVertex - Vertex) +
				glm::cross(RightVertex - Vertex, DownVertex - Vertex) + glm::cross(DownVertex - Vertex, LeftVertex - Vertex));

			InsertVertex(Sphere, (i * Segments + j + 2) * 6, Vertex, Normal);
		}
	}

	for (int i = 0; i < Rings - 1; ++i)
	{
		delete[] RCTSP[i];
	}
	delete[] RCTSP;
	for (int i = 0; i < Rings - 1; ++i)
	{
		delete[] RSTSP[i];
	}
	delete[] RSTSP;
	for (int i = 0; i < Rings - 1; ++i)
	{
		delete[] RCP[i];
	}
	delete[] RCP;

	SphereIndicesSize = 6 * Segments + 6 * (Rings - 2) * Segments;
	SphereIndices = new int[SphereIndicesSize];

	for (int i = 0; i < Segments; ++i)
	{
		InsertIndex(SphereIndices, i * 6, 0, i + 2, (i + 1) % Segments + 2);
		InsertIndex(SphereIndices, i * 6 + 3, 1, (Rings - 2) * Segments + (i + 1) % Segments + 2, (Rings - 2) * Segments + i + 2);
	}

	for (int i = 0; i < Rings - 2; ++i)
	{
		for (int j = 0; j < Segments; ++j)
		{
			InsertIndex(SphereIndices, Segments * 6 + (i * Segments + j) * 6, i * Segments + (j + 1) % Segments + 2, i * Segments + j + 2, (i + 1) * Segments + j + 2);
			InsertIndex(SphereIndices, Segments * 6 + (i * Segments + j) * 6 + 3, (i + 1) * Segments + j + 2, (i + 1) * Segments + (j + 1) % Segments + 2, i * Segments + (j + 1) % Segments + 2);
		}
	}
}

void GenerateGrid(float* &Grid, int* &GridIndices, int Vertices, float Range, int& GridSize, int& GridIndicesSize, float& SeparationFactor)
{
	int Size = 2 * Vertices + 1;
	GridSize = 2 * (int)glm::pow(Size, 2);
	Grid = new float[GridSize];

	SeparationFactor = Range / Size;

	for (int i = 0; i < Size; ++i)
	{
		for (int j = 0; j < Size; ++j)
		{
			InsertVertex2D(Grid, 2 * (i * Size + j), SeparationFactor * glm::vec2(j - Vertices, Vertices - i));
		}
	}

	GridIndicesSize = 6 * (int)glm::pow(Size - 1, 2);
	GridIndices = new int[GridIndicesSize];

	for (int i = 0; i < Size - 1; ++i)
	{
		for (int j = 0; j < Size - 1; ++j)
		{
			InsertIndex(GridIndices, 6 * (i * (Size - 1) + j), i * Size + j, (i + 1) * Size + j, i * Size + 1 + j);
			InsertIndex(GridIndices, 6 * (i * (Size - 1) + j) + 3, i * Size + 1 + j, (i + 1) * Size + j, (i + 1) * Size + 1 + j);
		}
	}
}