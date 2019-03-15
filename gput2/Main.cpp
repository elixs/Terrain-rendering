#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <utility>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Utils.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "resource.h"

// Show console
//#define  _CONSOLE

#ifdef _CONSOLE
#pragma comment( linker, "/SUBSYSTEM:CONSOLE" )
#else
#pragma comment( linker, "/SUBSYSTEM:WINDOWS" )
#endif

enum class EState
{
	OnGame,
	OnMenu,
	OnDemo
};
EState CurrentState = EState::OnGame;

// Init
void Init(GLFWwindow* &Window, const char* Title);
void ArrowInit(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, int &ArrowIndicesSize, int Vertices, float Radius, float Legth, void(*Generate)(float*&, int*&, int, float, float, int&, int&, bool));
void PointLightInit(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, int &PointLightIndicesSize, int Segments, int Rings, float Radius, void(*Generate)(float*&, int*&, int, int, float, int&, int&));
void GridInit(unsigned int &GridVAO, unsigned int &GridVBO, unsigned int &GridEBO, int &GridIndicesSize, float &SeparationFactor);

// Callbacks
void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
void KeyCallback(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods);
void CharacterCallback(GLFWwindow* Window, unsigned int Codepoint);
void MouseCallback(GLFWwindow* Window, double PositionX, double PositionY);
void ScrollCallback(GLFWwindow* Window, double OffsetX, double OffsetY);
void ProcessInput(GLFWwindow *Window);
void ErrorCallback(int Error, const char* Description);

// ImGui
bool SliderRotation(const char* label, void* v);
void ShowHelp();

// Settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
float LastX = SCR_WIDTH / 2.f;
float LastY = SCR_HEIGHT / 2.f;

GCamera Camera(glm::vec3(0.f, 12.f, 26.f));

bool FirstMouse = true;

float DeltaTime = 0.f;
float LastFrame = 0.f;
float TerrainTime = 10.f;

bool bShowHelp = true;
float FPSValues[120] = { 0 };
int FPSValuesOffset = 0;

bool bDLDemo = false;
bool bPLDemo = false;
bool bSLDemo = false;

#ifdef _CONSOLE
int main()
#else _WINDOWS
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	GLFWwindow* Window;
	Init(Window, u8"(/· - ·)/");

	// Shaders
	//GShader ArrowShader("Shaders/Arrow.vert", "Shaders/Arrow.frag");
	//GShader PointLightShader("Shaders/PointLight.vert", "Shaders/PointLight.frag");
	//GShader TerrainShader("Shaders/Terrain.vert", "Shaders/Terrain.frag");

	// To create stand-alone .exe
	GShader ArrowShader(ArrowVert, ArrowFrag);
	GShader PointLightShader(PointLightVert, PointLightFrag);
	GShader TerrainShader(TerrainVert, TerrainFrag);

	//// Directional Light Arrow
	//  Cylinder
	unsigned int CylinderVAO, CylinderVBO, CylinderEBO;
	int CylinderIndicesSize;
	ArrowInit(CylinderVAO, CylinderVBO, CylinderEBO, CylinderIndicesSize, 16, 0.01f, 0.2f, GenerateCylinder);

	// Cone
	unsigned int ConeVAO, ConeVBO, ConeEBO;
	int ConeIndicesSize;
	ArrowInit(ConeVAO, ConeVBO, ConeEBO, ConeIndicesSize, 16, 0.02f, 0.04f, GenerateCone);

	///// Terrain
	// Grid
	unsigned int GridVAO, GridVBO, GridEBO;
	int GridIndicesSize;
	float SeparationFactor;
	GridInit(GridVAO, GridVBO, GridEBO, GridIndicesSize, SeparationFactor);

	// PointLight
	unsigned int PointLightVAO, PointLightVBO, PointLightEBO;
	int PointLightIndicesSize;
	PointLightInit(PointLightVAO, PointLightVBO, PointLightEBO, PointLightIndicesSize, 32, 16, 1.f, GenerateSphere);

	ArrowShader.Use();

	ArrowShader.Set3f("UMaterial.Ambient", 0.1745f, 0.01175f, 0.01175f);
	ArrowShader.Set3f("UMaterial.Diffuse", 0.61424f, 0.04136f, 0.04136f);
	ArrowShader.Set3f("UMaterial.Specular", 0.727811f, 0.626959f, 0.626959f);
	ArrowShader.Set1f("UMaterial.Shininess", 76.8f);

	ArrowShader.SetVec3("USpotLight.Light.Ambient", glm::vec3(1.f));
	ArrowShader.SetVec3("USpotLight.Light.Diffuse", glm::vec3(1.f));
	ArrowShader.SetVec3("USpotLight.Light.Specular", glm::vec3(1.f));
	ArrowShader.Set1f("USpotLight.Constant", 1.f);
	ArrowShader.Set1f("USpotLight.Linear", 0.09f);
	ArrowShader.Set1f("USpotLight.Quadratic", 0.032f);
	ArrowShader.Set1f("USpotLight.CutOff", glm::cos(glm::radians(12.5f)));
	ArrowShader.Set1f("USpotLight.OuterCutOff", glm::cos(glm::radians(15.f)));

	TerrainShader.Use();
	TerrainShader.Set3f("UMaterial.Specular", 0.333333f, 0.333333f, 0.333333f);
	TerrainShader.Set3f("UMaterial.Emission", 1.f, 0.f, 0.f);
	TerrainShader.Set1f("UMaterial.Shininess", 9.84615f);

	TerrainShader.Set1f("USeparationFactor", SeparationFactor);

	//// ImGui variables
	ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Direction Light
	bool bUseDirectionalLight = true;
	glm::vec2 DLDirection(0.f, -45.f);
	glm::vec3 DLAmbient(0.5f);
	glm::vec3 DLDiffuse(0.5f);
	glm::vec3 DLSpectular(0.5f);

	// Point Light
	bool bUsePointLight = true;
	glm::vec3 PLPosition(0.f, 7.f, 10.f);
	glm::vec3 PLAmbient(0.25f);
	glm::vec3 PLDiffuse(0.75f);
	glm::vec3 PLSpectular(1.f);
	float PLConstant = 1.f;
	float PLLinear = 0.022f;
	float PLQuadratic = 0.0019f;

	// Spot Light
	bool bUseSpotLight = true;
	glm::vec3 SLAmbient(0.f);
	glm::vec3 SLDiffuse(1.f);
	glm::vec3 SLSpectular(1.f);
	float SLConstant = 1.f;
	float SLLinear = 0.09f;
	float SLQuadratic = 0.032f;
	float SLCutOff = 12.5f;
	float SLOuterCutOff = 15.f;

	// Terrain
	bool bTerrainWireframe = false;
	bool bTerrainLiveMotion = false;
	float TerrainMotionSpeed = 1.f;
	float Lenght = 10.f;
	float UHeight = 10.f;

	float CameraSpeed = Camera.MovementSpeed;

	while (!glfwWindowShouldClose(Window))
	{
		ImGui::CaptureMouseFromApp(false);
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (bShowHelp)
		{
			ShowHelp();
		}

		if (CurrentState == EState::OnMenu)
		{
			// Main GUI
			ImGui::SetNextWindowPos(ImVec2(350, 50), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(500, 0.f), ImGuiCond_Once);
			ImGui::Begin(u8"(~· - ·)~");                          // Create a window called "Hello, world!" and append into it.
			ImGui::ColorEdit3("Clear Color", (float*)&ClearColor); // Edit 3 floats representing a color
			ImGui::DragFloat("Camera Speed", &CameraSpeed, 0.1f);

			if (!ImGui::CollapsingHeader("Terrain"))
			{
				ImGui::Checkbox("Wireframe", &bTerrainWireframe); ImGui::SameLine(ImGui::GetContentRegionAvailWidth() > 300 ? 150 : ImGui::GetContentRegionAvailWidth() * 0.5f);
				ImGui::Checkbox("Live motion", &bTerrainLiveMotion);
				ImGui::SliderFloat("Motion Speed", &TerrainMotionSpeed, -2.5f, 2.5f);
				ImGui::SliderFloat("Lenght", &Lenght, 0.f, 100.f);
				ImGui::SliderFloat("Height", &UHeight, 0.f, 100.f);
			}
			if (!ImGui::CollapsingHeader("Directional Light"))
			{
				ImGui::PushID(0);
				ImGui::Checkbox("Active", &bUseDirectionalLight);
				SliderRotation("Direction", (float*)&DLDirection);
				ImGui::ColorEdit3("Ambient", (float*)&DLAmbient);
				ImGui::ColorEdit3("Diffuse", (float*)&DLDiffuse);
				ImGui::ColorEdit3("Specular", (float*)&DLSpectular);
				ImGui::PopID();
			}
			if (!ImGui::CollapsingHeader("Point Light"))
			{
				ImGui::PushID(1);
				ImGui::Checkbox("Active", &bUsePointLight);
				ImGui::DragFloat3("Position", (float*)&PLPosition, 0.1f);
				ImGui::ColorEdit3("Ambient", (float*)&PLAmbient);
				ImGui::ColorEdit3("Diffuse", (float*)&PLDiffuse);
				ImGui::ColorEdit3("Specular", (float*)&PLSpectular);
				ImGui::DragFloat("Constant", &PLConstant, 0.01f);
				ImGui::DragFloat("Linear", &PLLinear, 0.001f);
				ImGui::DragFloat("Quadratic", &PLQuadratic, 0.0001f, 0.f, 0.f, "%.4f");
				ImGui::PopID();
			}
			if (!ImGui::CollapsingHeader("Spot Light"))
			{
				ImGui::PushID(2);
				ImGui::Checkbox("Active", &bUseSpotLight);
				ImGui::ColorEdit3("Ambient", (float*)&SLAmbient);
				ImGui::ColorEdit3("Diffuse", (float*)&SLDiffuse);
				ImGui::ColorEdit3("Specular", (float*)&SLSpectular);
				ImGui::DragFloat("Constant", &SLConstant, 0.01f);
				ImGui::DragFloat("Linear", &SLLinear, 0.001f);
				ImGui::DragFloat("Quadratic", &SLQuadratic, 0.0001f, 0.f, 0.f, "%.4f");
				ImGui::DragFloat("Cut Off", &SLCutOff, 0.1f);
				ImGui::DragFloat("Outer Cut Off", &SLOuterCutOff, 0.1f);
				ImGui::PopID();
			}
			ImGui::End();

			// Demos
			ImGuiStyle& Style = ImGui::GetStyle();
			ImGuiContext* Context = ImGui::GetCurrentContext();

			//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
			//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

			ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			ImGui::SetNextWindowPos(ImVec2(0, 150), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Once);
			ImGui::Begin("Demos", NULL, WindowFlags);

			if (ImGui::Button("Directional Light Demo", ImVec2(300.f - 2.f * Style.WindowPadding.x, 0.f)))
			{
				CurrentState = EState::OnDemo;
				bDLDemo = true;
			}
			if (ImGui::Button("Point Light Demo", ImVec2(300.f - 2.f * Style.WindowPadding.x, 0.f)))
			{
				CurrentState = EState::OnDemo;
				bPLDemo = true;
			}
			if (ImGui::Button("Spot Light Demo", ImVec2(300.f - 2.f * Style.WindowPadding.x, 0.f)))
			{
				CurrentState = EState::OnDemo;
				bSLDemo = true;
			}

			ImGui::End();
		}

		float CurrentFrame = (float)glfwGetTime();
		DeltaTime = CurrentFrame - LastFrame;
		LastFrame = CurrentFrame;

		if (bTerrainLiveMotion)
		{
			TerrainTime += DeltaTime * TerrainMotionSpeed;
		}

		Camera.MovementSpeed = CameraSpeed;

		// Demos
		if (bDLDemo)
		{
			Camera.Position = glm::vec3(0.f, 12.f, 26.f);
			Camera.Yaw = -90.f;
			Camera.Pitch = -23.f;
			Camera.WorldUp = glm::vec3(0.f, 1.f, 0.f);
			Camera.UpdateCameraVectors();
			DLDirection = glm::vec2(45.f * glm::sin(CurrentFrame), -90.f + 22.5f * glm::cos(CurrentFrame));

			bUseDirectionalLight = true;
			bUsePointLight = false;
			bUseSpotLight = false;

			// Terrain
			bTerrainWireframe = false;
			bTerrainLiveMotion = false;
			Lenght = 10.f;
			UHeight = 10.f;

			// Direction Light
			DLAmbient = glm::vec3(0.5f);
			DLDiffuse = glm::vec3(0.5f);
			DLSpectular = glm::vec3(0.5f);
		}
		if (bPLDemo)
		{
			Camera.Position = glm::vec3(0.f, 12.f, 26.f);
			Camera.Yaw = -90.f;
			Camera.Pitch = -23.f;
			Camera.WorldUp = glm::vec3(0.f, 1.f, 0.f);
			Camera.UpdateCameraVectors();
			float PLPX = 2.2f * glm::sin(3.5f * CurrentFrame) + 3.8f * glm::cos(1.4f * CurrentFrame) + 5.3f * glm::sin(2.6f * CurrentFrame);
			float PLPZ = 7.f + 3.7f * glm::sin(2.6f * CurrentFrame) + 2.1f * glm::cos(1.9f * CurrentFrame) + 3.8f * glm::cos(2.4f * CurrentFrame);
			PLPosition = glm::vec3(PLPX, 9.f, PLPZ);

			bUseDirectionalLight = false;
			bUsePointLight = true;
			bUseSpotLight = false;

			// Terrain
			bTerrainWireframe = false;
			bTerrainLiveMotion = false;
			Lenght = 10.f;
			UHeight = 10.f;

			// Point Light
			PLAmbient = glm::vec3(0.25f);
			PLDiffuse = glm::vec3(0.75f);
			PLSpectular = glm::vec3(1.f);
			PLConstant = 1.f;
			PLLinear = 0.022f;
			PLQuadratic = 0.0019f;
		}
		if (bSLDemo)
		{
			Camera.Position = glm::vec3(0.f, 10.f, 0.f);
			Camera.Yaw = CurrentFrame * 3.f;
			Camera.Pitch = -28.f;
			Camera.WorldUp = glm::vec3(0.f, 1.f, 0.f);
			Camera.UpdateCameraVectors();

			bUseDirectionalLight = false;
			bUsePointLight = false;
			bUseSpotLight = true;

			// Terrain
			bTerrainWireframe = false;
			bTerrainLiveMotion = false;
			Lenght = 10.f;
			UHeight = 10.f;

			// Spot Light
			SLAmbient = glm::vec3(0.f);
			SLDiffuse = glm::vec3(1.f);
			SLSpectular = glm::vec3(1.f);
			SLConstant = 1.f;
			SLLinear = 0.09f;
			SLQuadratic = 0.032f;
			SLCutOff = 12.5f;
			SLOuterCutOff = 15.f;
		}

		ProcessInput(Window);

		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int Width, Height;
		glfwGetFramebufferSize(Window, &Width, &Height);

		glm::mat4 Projection = glm::perspective(glm::radians(Camera.Zoom), (float)Width / (float)Height, 0.1f, 100.f);
		glm::mat4 View = Camera.GetViewMatrix();
		glm::mat4 Model(1.f);

		// TerrainShader
		glBindVertexArray(GridVAO);
		TerrainShader.Use();

		//// Lights
		// Directional Light
		if (bUseDirectionalLight)
		{
			TerrainShader.SetVec3("UDirectionalLight.Light.Ambient", DLAmbient);
			TerrainShader.SetVec3("UDirectionalLight.Light.Diffuse", DLDiffuse);
			TerrainShader.SetVec3("UDirectionalLight.Light.Specular", DLSpectular);
		}
		else
		{
			TerrainShader.SetVec3("UDirectionalLight.Light.Ambient", glm::vec3(0.f));
			TerrainShader.SetVec3("UDirectionalLight.Light.Diffuse", glm::vec3(0.f));
			TerrainShader.SetVec3("UDirectionalLight.Light.Specular", glm::vec3(0.f));
		}
		TerrainShader.SetVec2r("UDirectionalLight.Direction", DLDirection);

		// Point Light
		if (bUsePointLight)
		{
			TerrainShader.SetVec3("UPointLights[0].Light.Ambient", PLAmbient);
			TerrainShader.SetVec3("UPointLights[0].Light.Diffuse", PLDiffuse);
			TerrainShader.SetVec3("UPointLights[0].Light.Specular", PLSpectular);
		}
		else
		{
			TerrainShader.SetVec3("UPointLights[0].Light.Ambient", glm::vec3(0.f));
			TerrainShader.SetVec3("UPointLights[0].Light.Diffuse", glm::vec3(0.f));
			TerrainShader.SetVec3("UPointLights[0].Light.Specular", glm::vec3(0.f));
		}
		TerrainShader.SetVec3("UPointLights[0].Position", PLPosition);
		TerrainShader.Set1f("UPointLights[0].Constant", PLConstant);
		TerrainShader.Set1f("UPointLights[0].Linear", PLLinear);
		TerrainShader.Set1f("UPointLights[0].Quadratic", PLQuadratic);

		// Spot Light
		if (bUseSpotLight)
		{
			TerrainShader.SetVec3("USpotLight.Light.Ambient", SLAmbient);
			TerrainShader.SetVec3("USpotLight.Light.Diffuse", SLDiffuse);
			TerrainShader.SetVec3("USpotLight.Light.Specular", SLSpectular);
		}
		else
		{
			TerrainShader.SetVec3("USpotLight.Light.Ambient", glm::vec3(0.f));
			TerrainShader.SetVec3("USpotLight.Light.Diffuse", glm::vec3(0.f));
			TerrainShader.SetVec3("USpotLight.Light.Specular", glm::vec3(0.f));
		}
		TerrainShader.SetVec3("USpotLight.Position", Camera.Position);
		TerrainShader.SetVec3("USpotLight.Direction", Camera.Front);
		TerrainShader.Set1f("USpotLight.Constant", SLConstant);
		TerrainShader.Set1f("USpotLight.Linear", SLLinear);
		TerrainShader.Set1f("USpotLight.Quadratic", SLQuadratic);
		TerrainShader.Set1f("USpotLight.CutOff", glm::cos(glm::radians(SLCutOff)));
		TerrainShader.Set1f("USpotLight.OuterCutOff", glm::cos(glm::radians(SLOuterCutOff)));

		TerrainShader.SetVec3("UViewPosition", Camera.Position);

		TerrainShader.SetMat4("UProjection", Projection);
		TerrainShader.SetMat4("UView", View);
		TerrainShader.SetMat4("UModel", Model);

		TerrainShader.Set1f("UWidth", Lenght);
		TerrainShader.Set1f("UHeight", UHeight);
		TerrainShader.Set1f("UTime", TerrainTime);

		if (bTerrainWireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		glDrawElements(GL_TRIANGLES, GridIndicesSize, GL_UNSIGNED_INT, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (bUsePointLight)
		{
			PointLightShader.Use();

			PointLightShader.SetVec3("UViewPosition", Camera.Position);

			PointLightShader.SetMat4("UProjection", Projection);
			PointLightShader.SetMat4("UView", View);
			Model = glm::mat4(1.f);
			Model = glm::translate(Model, PLPosition);
			PointLightShader.SetMat4("UModel", Model);

			glBindVertexArray(PointLightVAO);
			glDrawElements(GL_TRIANGLES, PointLightIndicesSize, GL_UNSIGNED_INT, 0);
		}

		glClear(GL_DEPTH_BUFFER_BIT);

		if (bUseDirectionalLight)
		{
			ArrowShader.Use();

			ArrowShader.SetVec3("USpotLight.Position", Camera.Position);
			ArrowShader.SetVec3("USpotLight.Direction", 2.f * Camera.Front + glm::vec3(0.f, 0.5f, 0.f));

			ArrowShader.SetVec3("UViewPosition", Camera.Position);

			ArrowShader.SetMat4("UProjection", Projection);
			ArrowShader.SetMat4("UView", View);

			Model = glm::mat4(1.f);
			Model = glm::translate(Model, Camera.Position + 2.f * Camera.Front + glm::vec3(0.f, 0.5f, 0.f));

			glm::vec3 Direction = -GetNormal(DLDirection);
			glm::vec3 Front = Direction;
			glm::vec3 Right = glm::normalize(glm::cross(Front, glm::vec3(0.f, 1.f, 0.f)));
			glm::vec3 Up = glm::normalize(glm::cross(Right, Front));

			glm::mat4 Rotation = glm::inverse(glm::lookAt(glm::vec3(0.f, 0.f, 0.f), glm::vec3(Direction.x, Direction.y, Direction.z), Up));

			Model = Model * Rotation;
			Model = glm::rotate(Model, glm::radians(270.f), glm::vec3(0.f, 1.f, 0.f));
			Model = glm::rotate(Model, glm::radians(270.f), glm::vec3(0.f, 0.f, 1.f));

			ArrowShader.SetMat4("UModel", Model);
			glBindVertexArray(CylinderVAO);
			glDrawElements(GL_TRIANGLES, CylinderIndicesSize, GL_UNSIGNED_INT, 0);

			Model = glm::translate(Model, glm::vec3(0.f, 0.1f, 0.f));
			ArrowShader.SetMat4("UModel", Model);
			glBindVertexArray(ConeVAO);
			glDrawElements(GL_TRIANGLES, ConeIndicesSize, GL_UNSIGNED_INT, 0);
		}

		// Rendering
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &PointLightVAO);
	glDeleteBuffers(1, &PointLightVBO);
	glDeleteBuffers(1, &PointLightEBO);

	glDeleteVertexArrays(1, &CylinderVAO);
	glDeleteBuffers(1, &CylinderVBO);
	glDeleteBuffers(1, &CylinderEBO);

	glDeleteVertexArrays(1, &GridVAO);
	glDeleteBuffers(1, &GridVBO);
	glDeleteBuffers(1, &GridEBO);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(Window);
	glfwTerminate();
	return 0;
}

void Init(GLFWwindow* &Window, const char* Title)
{
	glfwSetErrorCallback(ErrorCallback);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	Window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, Title, NULL, NULL);
	if (Window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(Window);

	// Callbacks
	glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
	glfwSetKeyCallback(Window, KeyCallback);
	glfwSetCharCallback(Window, CharacterCallback);
	glfwSetCursorPosCallback(Window, MouseCallback);
	glfwSetScrollCallback(Window, ScrollCallback);

	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	io.IniFilename = "imgui/imgui.ini";

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(Window, false);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	// Setup Style
	ImGui::StyleColorsClassic();

	glEnable(GL_DEPTH_TEST);
}

void ArrowInit(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, int &ArrowIndicesSize, int Vertices, float Radius, float Legth, void(*Generate)(float*&, int*&, int, float, float, int&, int&, bool))
{
	// in
	float* Arrow;
	int ArrowSize;

	// out
	int* ArrowIndices;
	//int ArrowIndicesSize;

	Generate(Arrow, ArrowIndices, Vertices, Radius, Legth, ArrowSize, ArrowIndicesSize, true);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, ArrowSize * sizeof(float), Arrow, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ArrowIndicesSize * sizeof(float), ArrowIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void PointLightInit(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, int &PointLightIndicesSize, int Segments, int Rings, float Radius, void(*Generate)(float*&, int*&, int, int, float, int&, int&))
{
	// in
	float* PointLight;
	int PointLightSize;

	// out
	int* PointLightIndices;
	//int PointLightIndicesSize;

	Generate(PointLight, PointLightIndices, Segments, Rings, Radius, PointLightSize, PointLightIndicesSize);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, PointLightSize * sizeof(float), PointLight, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, PointLightIndicesSize * sizeof(float), PointLightIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void GridInit(unsigned int &GridVAO, unsigned int &GridVBO, unsigned int &GridEBO, int &GridIndicesSize, float &SeparationFactor)
{
	// in
	float* Grid;
	int GridSize;

	// out
	int* GridIndices;
	//int GridIndicesSize;
	//float SeparationFactor;

	GenerateGrid(Grid, GridIndices, 500, 5.f, GridSize, GridIndicesSize, SeparationFactor);

	// Grid
	glGenVertexArrays(1, &GridVAO);
	glGenBuffers(1, &GridVBO);
	glGenBuffers(1, &GridEBO);

	glBindVertexArray(GridVAO);

	glBindBuffer(GL_ARRAY_BUFFER, GridVBO);
	glBufferData(GL_ARRAY_BUFFER, GridSize * sizeof(float), Grid, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GridEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, GridIndicesSize * sizeof(float), GridIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void ProcessInput(GLFWwindow *Window)
{
	if (CurrentState == EState::OnGame)
	{
		if (glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			DeltaTime *= 3.f;
		}
		if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
		{
			Camera.ProcessKeyboard(ECameraMovement::Forward, DeltaTime);
		}
		if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Camera.ProcessKeyboard(ECameraMovement::Backward, DeltaTime);
		}
		if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Camera.ProcessKeyboard(ECameraMovement::Left, DeltaTime);
		}
		if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Camera.ProcessKeyboard(ECameraMovement::Right, DeltaTime);
		}
		if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
		{
			Camera.ProcessKeyboard(ECameraMovement::Up, DeltaTime);
		}
		if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			Camera.ProcessKeyboard(ECameraMovement::Down, DeltaTime);
		}
	}
}

// Callbacks

void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

void KeyCallback(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{
	if (CurrentState == EState::OnGame)
	{
		if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
		{
			//glfwSetWindowShouldClose(Window, true);
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			CurrentState = EState::OnMenu;
		}
		if (Key == GLFW_KEY_R && Action == GLFW_PRESS)
		{
			TerrainTime += 1000.f;
		}
	}
	else if (CurrentState == EState::OnMenu)
	{
		if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			CurrentState = EState::OnGame;
			FirstMouse = true;
		}
		ImGui_ImplGlfw_KeyCallback(Window, Key, ScanCode, Action, Mods);
	}
	else if (CurrentState == EState::OnDemo)
	{
		if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
		{
			CurrentState = EState::OnMenu;
			bDLDemo = false;
			bPLDemo = false;
			bSLDemo = false;
		}
	}

	if (Key == GLFW_KEY_H && Action == GLFW_PRESS)
	{
		bShowHelp = !bShowHelp;
	}
}

void CharacterCallback(GLFWwindow* Window, unsigned int Codepoint)
{
	if (CurrentState == EState::OnMenu)
	{
		ImGui_ImplGlfw_CharCallback(Window, Codepoint);
	}
}

void MouseCallback(GLFWwindow* Window, double PositionX, double PositionY)
{
	if (CurrentState == EState::OnGame)
	{
		if (FirstMouse)
		{
			LastX = (float)PositionX;
			LastY = (float)PositionY;
			FirstMouse = false;
		}

		float OffsetX = (float)PositionX - LastX;
		float OffsetY = LastY - (float)PositionY; // reversed since y-coordinates go from bottom to top

		LastX = (float)PositionX;
		LastY = (float)PositionY;

		Camera.ProcessMouseMovement(OffsetX, OffsetY);
	}
}

void ScrollCallback(GLFWwindow* Window, double OffsetX, double OffsetY)
{
	if (CurrentState == EState::OnGame)
	{
		Camera.ProcessMouseScroll((float)OffsetY);
	}
	else if (CurrentState == EState::OnMenu)
	{
		ImGui_ImplGlfw_ScrollCallback(Window, OffsetX, OffsetY);
	}
}

void ErrorCallback(int Error, const char* Description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", Error, Description);
}

// ImGui

bool SliderRotation(const char* label, void* v)
{
	ImGuiWindow* Window = ImGui::GetCurrentWindow();

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	ImGui::PushMultiItemsWidths(2);
	size_t type_size = sizeof(float);
	const char* Formats[3] = { "Yaw:%.3f" , "Pitch:%.3f" };
	float v_min = -360.f;
	float v_max = 360.f;
	for (int i = 0; i < 2; i++)
	{
		ImGui::PushID(i);
		value_changed |= ImGui::SliderScalar("##v", ImGuiDataType_Float, v, &v_min, &v_max, Formats[i], 1.f);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
		v = (void*)((char*)v + type_size);
	}
	ImGui::PopID();

	ImGui::TextUnformatted(label, ImGui::FindRenderedTextEnd(label));
	ImGui::EndGroup();
	return value_changed;
}

void ShowHelp()
{
	std::ostringstream X;
	X.precision(3);
	X << std::fixed;
	X << "X: " << Camera.Position.x;

	std::ostringstream Y;
	Y.precision(3);
	Y << std::fixed;
	Y << "Y: " << Camera.Position.y;

	std::ostringstream Z;
	Z.precision(3);
	Z << std::fixed;
	Z << "Z: " << Camera.Position.z;

	std::ostringstream Yaw;
	Yaw.precision(3);
	Yaw << std::fixed;
	Yaw << "Yaw: " << Camera.Yaw;

	std::ostringstream Pitch;
	Pitch.precision(3);
	Pitch << std::fixed;
	Pitch << "Pitch: " << Camera.Pitch;

	std::ostringstream FOV;
	FOV << "FOV: " << Camera.Zoom;

	std::ostringstream FPS;
	FPS << "FPS: " << ImGui::GetIO().Framerate;

	ImGuiStyle& Style = ImGui::GetStyle();
	ImGuiContext* Context = ImGui::GetCurrentContext();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 0.f), ImGuiCond_Once);
	ImGui::Begin("Info", NULL, WindowFlags);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.f, 0.f, 1.f));
	ImGui::Button(X.str().c_str(), ImVec2(100.f, 0.f)); ImGui::SameLine();
	ImGui::PopStyleColor(3);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 1.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 1.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 1.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.f, 0.f, 1.f));
	ImGui::Button(Y.str().c_str(), ImVec2(100.f, 0.f)); ImGui::SameLine();
	ImGui::PopStyleColor(4);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 1.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 1.f, 1.f));
	ImGui::Button(Z.str().c_str(), ImVec2(100.f, 0.f));
	ImGui::PopStyleColor(3);

	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Style.Colors[ImGuiCol_Button]);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, Style.Colors[ImGuiCol_Button]);

	ImGui::Button(Yaw.str().c_str(), ImVec2(150.f, 0.f)); ImGui::SameLine();

	ImGui::Button(Pitch.str().c_str(), ImVec2(150.f, 0.f));

	ImGui::Button(FOV.str().c_str(), ImVec2(300.f, 0.f));

	ImGui::PopStyleColor(2);

	FPSValues[FPSValuesOffset] = ImGui::GetIO().Framerate;
	FPSValuesOffset = (FPSValuesOffset + 1) % IM_ARRAYSIZE(FPSValues);
	ImGui::PlotLines("Lines", FPSValues, IM_ARRAYSIZE(FPSValues), FPSValuesOffset, FPS.str().c_str(), FLT_MAX, FLT_MAX, ImVec2(300.f, 2.f * (Context->FontSize + Style.FramePadding.y * 2.f)));

	ImGui::End();

	ImGui::PopStyleVar(4);
}