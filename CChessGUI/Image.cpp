#include "Image.h"

#include <cstdint>
#include <cstring>
#include <format>
#include <GL/glew.h>
#include <stdexcept>
#include <string_view>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



//	Public Methods

//constructors
Image::Image(std::string_view image)
{
	int channels{};

	//stbi_set_flip_vertically_on_load(true);

	m_data = stbi_load(image.data(), &m_width, &m_height, &channels, 0);
	if (!m_data) throw std::runtime_error(std::format("could not find image {}", image));

	m_format = (channels == 4 ? GL_RGBA : GL_RGB);
}

Image::Image(const Image& other) noexcept
	: m_data(new std::uint8_t[other.size()]), m_width(other.m_width), m_height(other.m_height), m_format(other.m_format)
{
	std::memcpy(m_data, other.m_data, other.size());
}

Image::Image(Image&& other) noexcept
	: m_data(std::exchange(other.m_data, nullptr)), m_width(std::exchange(other.m_width, 0)), m_height(std::exchange(other.m_height, 0)), m_format(std::exchange(other.m_format, 0)) { }

Image& Image::operator= (const Image& other) noexcept
{
	stbi_image_free(m_data);
	m_data = new std::uint8_t[other.size()];
	std::memcpy(m_data, other.m_data, other.size());

	m_width = other.m_width;
	m_height - other.m_height;
	m_format = other.m_format;

	return *this;
}

Image& Image::operator= (Image&& other) noexcept
{
	stbi_image_free(m_data);

	m_data = std::exchange(other.m_data, nullptr);
	m_width = std::exchange(other.m_width, 0);
	m_height = std::exchange(other.m_height, 0);
	m_format = std::exchange(other.m_format, 0);

	return *this;
}

Image::~Image()
{
	stbi_image_free(m_data);
}



//getters
const std::uint8_t* Image::data() const noexcept
{
	return m_data;
}

GLuint Image::format() const noexcept
{
	return m_format;
}

int Image::width() const noexcept
{
	return m_width;
}

int Image::height() const noexcept
{
	return m_height;
}

std::size_t Image::size() const noexcept
{
	return static_cast<std::size_t>(m_width) * static_cast<std::size_t>(m_height);
}