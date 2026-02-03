#pragma once

#include <GL/glew.h>
#include <string_view>
#include <cstddef>

#include "Image.h"



class Texture
{
private:

	GLuint m_texture;



private:

	//private methods
	void init() noexcept;



public:

	//constructors
	Texture() noexcept;

	Texture(std::string_view texture);

	Texture(const Image& texture);

	Texture(Texture& other) = delete;

	Texture(Texture&& other) noexcept;

	Texture& operator= (Texture& other) = delete;

	Texture& operator= (Texture&& other) noexcept;

	~Texture();



	//public methods
	void bind() const noexcept;
};

