#pragma once

#include <string_view>
#include <GL/glew.h>
#include <cstdint>



class Image
{
private:

	//	Private Members
	std::uint8_t* m_data{};
	int m_width{};
	int m_height{};
	GLuint m_format{};



public:

	//	Public Methods

	//constructors
	Image(std::string_view image);

	Image(const Image& other) noexcept;

	Image(Image&& other) noexcept;

	Image& operator= (const Image& other) noexcept;

	Image& operator= (Image&& other) noexcept;

	~Image();



	//getters
	const std::uint8_t* data() const noexcept;

	GLuint format() const noexcept;

	int width() const noexcept;

	int height() const noexcept;

	std::size_t size() const noexcept;
};