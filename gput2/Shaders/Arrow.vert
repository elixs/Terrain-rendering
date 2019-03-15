#version 330 core

layout (location = 0) in vec3 VPosition;
layout (location = 1) in vec3 VNormal;

uniform mat4 UModel;
uniform mat4 UView;
uniform mat4 UProjection;

out vec3 FPosition;
out vec3 FNormal;

void main()
{
	FPosition = vec3(UModel * vec4(VPosition, 1.f));
	FNormal = mat3(transpose(inverse(UModel))) * VNormal;

	gl_Position = UProjection * UView * UModel * vec4(VPosition, 1.f);
}