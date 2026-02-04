#pragma once

#include <cstdint>
#include <GL/glew.h>
#include <span>
#include <string_view>



class Texture
{
private:

	//	Private Members

	GLuint m_texture;
	int m_width{};
	int m_height{};



private:

	//	Private Methods

	void init() noexcept;



public:

	//	Public Methods

	//constructors
	Texture(std::string_view image);

	Texture(std::span<const std::uint8_t> data, int width, int height) noexcept;

	Texture(Texture& other) = delete;

	Texture(Texture&& other) noexcept;

	Texture& operator= (Texture& other) = delete;

	Texture& operator= (Texture&& other) noexcept;

	~Texture();



	//setters
	void bind() const noexcept;

	void update(std::span<const std::uint8_t> data) const noexcept;
};

