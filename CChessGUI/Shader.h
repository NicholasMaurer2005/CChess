#pragma once

#include <GL/glew.h>
#include <string_view>



class Shader
{
public:

	//	Public Definitions
	
	// This is a violation of my naming convention. This is so I can match the GLSL type vec2. I did not want to add all of GLM just
	// for glm::vec2.

private:

	//	Private Members

	GLuint m_program{};



public:

	//	Public Methods
	 
	//constructors
	Shader() noexcept {};

	Shader(std::string_view vertex, std::string_view fragment);

	Shader(const Shader& other) = delete;

	Shader(Shader&& other) noexcept;

	Shader& operator= (const Shader& other) = delete;

	Shader& operator= (Shader&& other) noexcept;

	~Shader();



	//getters
	GLint uniformLocation(std::string_view uniform) const;



	//setters
	void use() const noexcept;

	void uniformVec2(GLint location, float x, float y) const noexcept;
};

