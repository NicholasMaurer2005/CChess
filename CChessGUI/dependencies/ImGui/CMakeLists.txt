# create the library
add_library(ImGui)

# add source files
target_sources(ImGui PRIVATE
	src/imgui.cpp
	src/imgui_demo.cpp
	src/imgui_draw.cpp
	src/imgui_impl_glfw.cpp
	src/imgui_impl_opengl3.cpp
	src/imgui_tables.cpp
	src/imgui_widgets.cpp
)

# add include directories
target_include_directories(ImGui
	PUBLIC include
	PRIVATE 
		src
		"${CMAKE_SOURCE_DIR}/CChessGUI/dependencies/GLFW/include"
)