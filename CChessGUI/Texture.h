#pragma once

#include <cstdint>
#include <GL/glew.h>
#include <span>
#include <string_view>



class Texture
{
public:

	//	Public Definitions

	struct alignas(4) Pixel
	{
		std::uint8_t r, g, b, a;
	};

	enum class MagFilter
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR
	};

private:

	//	Private Members

	GLuint m_texture{};
	int m_width{};
	int m_height{};



private:

	//	Private Methods

	void init(MagFilter magFilter) noexcept;



public:

	//	Public Methods

	//constructors
	Texture() noexcept {};

	Texture(std::string_view image, MagFilter magFilter);

	Texture(std::span<const Pixel> data, int width, int height, MagFilter magFilter) noexcept;

	Texture(Texture& other) = delete;

	Texture(Texture&& other) noexcept;

	Texture& operator= (Texture& other) = delete;

	Texture& operator= (Texture&& other) noexcept;

	~Texture();



	//setters
	void bind() const noexcept;

	void update(std::span<const Pixel> data) const noexcept;
};

