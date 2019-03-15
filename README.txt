La aplicaci�n fue creada utiizando Visual Studio 2017, se adjunta el proyecto completo. Para los shader se utiliz� OpenGL 3.3 core. Se adjunta igualmente un .exe para probar el programa sin necesidad de establecer dependencias.

Controles de la aplicaci�n:

WASDQE		Moverse
Mouse		Mover c�mara
Scroll		Zoom
Shift		Mantener apretado para moverse m�s r�pido
R			Generar nuevo terreno
H			Ocultar Info
Esc			Menu

Dentro del Men� se puede acceder a las tres demos incorporadas, saliendo de ellas con Esc. Tambi�n se puede cambiar cada uno de los par�metros de manera independiente.

Sugerencias: Probar las tres demos y el ticket Live Motion del Men�

Las dependencias que se utilizan son GLFW para el manejo de ventanas, GLAD para cargar las funciones de OpenGL y GLM para funciones matem�ticas. Se pueden encontrar buenos tutoriales de como incorporar estas bibliotecas a Visual Studio en https://learnopengl.com/Getting-started/Creating-a-window para las primeras dos y al final de https://learnopengl.com/Getting-started/Transformations para la tercera. De todas maneras se proceder� a explicar los pasos generales para utilizar estas dependencias.

* Requisitos:
- Visual Studio 2017: https://visualstudio.microsoft.com/downloads/
- CMake (Win64 Installer): https://cmake.org/download/
- GLFW (Source package): http://www.glfw.org/download.html
- Glad (Language: C/C++; Specification: Opengl; gl: >= 3.3; Profile: Core; Generate Loader: True): https://glad.dav1d.de/

* Visual Studio
- Instalar Visual Studio con la opci�n "Desktop development with C++"

* CMake
- Instalar CMake

* Crear Carpetas:
- En un path a elecci�n (ej: Documents) crear una carpeta llamada OpenGL. Dentro de esta crear dos carpetas /Include y /Libraries.

* GLFW
- Descomprimir .zip descargado (ver que no cree una carpeta que dentro tenga solo una carpeta del mismo nombre, si es asi tirar el contenido para afuera y borrar la carpeta extra)
- Abrir CMAKE
- En "Where is the source code:" elegir la carpeta descargada
- En "Where to build the binaries:" usar el mismo path anterior agregando /build
- Apretar Configure
- Aceptar la creaci�n de directorio
- Elegir Visual Studio 15 2017 Win64
- Apretar Finish
- Esperar
- Apretar Configure
- Apretar Generate
- Apretar Open Project
- Cambiar en Visual Studio Debug por Release y ver que diga x64
- Build -> Buil Solution
- Esperar
- Ir al path del .zip descomprimido
- Copiar el archivo /build/src/glfw.lib en /Libraries
- Copiar /include/GLFW en /Include

* GLAD
- Descomprimir .zip descargado (ver que no cree una carpeta que dentro tenga solo una carpeta del mismo nombre, si es asi tirar el contenido para afuera y borrar la carpeta extra)
- Copiar el contenido de /include a /Include
- Reemplazar el archivo glad.c del proyecto (desde el path de este archivo es gput2/glad.c) por el archivo src/glad.c

* GLM
- Descomprimir .zip descargado (ver que no cree una carpeta que dentro tenga solo una carpeta del mismo nombre, si es asi tirar el contenido para afuera y borrar la carpeta extra)
- Copiar la capeta glm/glm a /Include

* Linkear dependencias
- Abrir el proyecto (desde el path de este archivo es gput2.sln)
- Project -> gput2 Properties...
- Configuration Properties -> VC++ Directories -> Include Directories -> Dropdown de la derecha -> <Edit...> -> S�mbolo de nueva carpeta -> Tres punto de la derecha -> Buscar la carpeta /Include -> Select Folder -> OK
- Configuration Properties -> VC++ Directories -> Library Directories -> Dropdown de la derecha -> <Edit...> -> S�mbolo de nueva carpeta -> Tres punto de la derecha -> Buscar la carpeta /Libraries -> Select Folder -> OK
- Configuration Properties -> Linker -> Input -> Additional Dependencies-> Dropdown de la derecha -> <Edit...> -> Pegar

opengl32.lib
glfw3.lib

en el primer cuadro de texto -> OK -> OK

* Ya con las dependencia agregadas solo queda eligir Release y x64 y ejecutar el proyecto
