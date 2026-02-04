#pragma once

#include <GL/glew.h>
#include <string_view>



class Shader
{
private:

	//	Private Members

	GLuint m_program{};



public:

	//	Public Methods
	 
	//constructors
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
};

