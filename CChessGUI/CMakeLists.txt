# create an executable
add_executable(CChessGUI)

# preprocessor definitions
target_compile_definitions(CChessGUI PRIVATE GLEW_STATIC)

# add GLFW
add_library(GLFW STATIC IMPORTED)
set_target_properties(GLFW PROPERTIES 
	IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLFW/lib/glfw3.lib"
	)
	
# add GLEW
add_library(GLEW STATIC IMPORTED)
set_target_properties(GLFW PROPERTIES 
	IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}dependencies//GLEW/lib/glew32s.lib"
	)

# add libraries
target_link_libraries(CChessGUI PRIVATE
	CChess
	GLFW
	GLEW
	ImGui
)

# add source files
target_sources(CChessGUI PRIVATE
	src/Buffer.cpp
	src/CChessGUI.cpp
	src/Image.cpp
	src/Main.cpp
	src/MenuManager.cpp
	src/PieceSprite.cpp
	src/Shader.cpp
	src/Texture.cpp
	src/Window.cpp
)

# add include directories
target_include_directories(CChessGUI PRIVATE
	src
	dependencies/GLFW/include
	dependencies/GLEW/include
	dependencies/ImGui/include
	dependencies/stb_image/include
)
