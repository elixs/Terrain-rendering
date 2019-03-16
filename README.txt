The application was created using Visual Studio 2017. For the shaders it's used OpengGL 3.3 core. There is a .exe to test the program without installing dependencies.

Application controls:

WASDQE    Movement
Mouse     Camera movement
Scroll    Zoom
Shift     Hold to move faster
R         Generate new terrain
H         Hide info
Esc       Access/exit interactive menu

On the menu there is three demos to test each one of the different light types.

Suggestions: Test the three demos and the Live Motion option on the menu.

The dependencies used are GLFW for window managment, GLAD to load OpenGL functions and GLM for mathematics. You can find good tutorials of how install this libraries into Visual Studio on https://learnopengl.com/Getting-started/Creating-a-window for the first two and https://learnopengl.com/Getting-started/Transformations for the last one. Either way here is a brief explanation of how to install this dependencies:

* Requirements:
- Visual Studio 2017: https://visualstudio.microsoft.com/downloads/
- CMake (Win64 Installer): https://cmake.org/download/
- GLFW (Source package): http://www.glfw.org/download.html
- GLAD (Language: C/C++; Specification: Opengl; gl: >= 3.3; Profile: Core; Generate Loader: True): https://glad.dav1d.de/

* Visual Studio
- Install Visual Studio choosing the option "Desktop development with C++"

* CMake
- Install CMake

* Creating Folders:
- On a path (ej: Documents) create a folder /OpenGL. Inside this folder create two more folders /Include and /Libraries.

* GLFW
- Decompress the downloaded .zip (make sure that the decompression don't create only a folder with the same name, if this is the case move all the content outside that folder and delete it)
- Open CMAKE
- On "Where is the source code:" choose the downloaded folder.
- On "Where to build the binaries:" use the same path of the last step but adding /build
- Click Configure
- Confirm the directory creation
- Choose Visual Studio 15 2017 Win64
- Click Finish
- Wait
- Click Configure
- Click Generate
- Click Open Project
- On Visual Studio change Debug for Release and confirm that the project builds for x64
- Build -> Build Solution
- Wait
- Go to the decompressed .zip path
- Copy the file /build/src/glfw.lib to /Libraries
- Copy the folder /include/GLFW to /Include

* GLAD
- Decompress the downloaded .zip (make sure that the decompression don't create only a folder with the same name, if this is the case move all the content outside that folder and delete it)
- Copy the content of /include to /Include
- Overwrite the glad.c project file (from this file path gput2/glad.c) with the src/glad.c file

* GLM
- Decompress the downloaded .zip (make sure that the decompression don't create only a folder with the same name, if this is the case move all the content outside that folder and delete it)
- Copy the glm/glm folder to /Include

* Dependencies linking
- Open the project (from this file path gput2.sln)
- Project -> gput2 Properties...
- Configuration Properties -> VC++ Directories -> Include Directories -> Dropdown on the right -> <Edit...> -> New folder icon -> Three dots on the right -> Search /Include folder-> Select Folder -> OK
- Configuration Properties -> VC++ Directories -> Library Directories -> Dropdown on the right -> <Edit...> -> New folder icon -> Three dots on the right -> Search /Libraries folder -> Select Folder -> OK
- Configuration Properties -> Linker -> Input -> Additional Dependencies-> Dropdown on the right -> <Edit...> -> Paste

opengl32.lib
glfw3.lib

on the first text box -> OK -> OK

* Now with the appropriate dependencies installed the only thing left to do is choose Release y x64 for the build and run the project
