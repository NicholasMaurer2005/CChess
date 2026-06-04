# create the library
add_library(ImGui)

# add source files
target_sources(ImGui PRIVATE
	imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_impl_glfw3.cpp
	imgui_impl_opengl3.cpp
	imgui_tables.cpp
	imgui_widgets.cpp
)

# add include directories
target_include_directories(Cchess
	PUBLIC include
	PRIVATE src
)