#include "Texture.h"

#include <GL/glew.h>
#include <span>
#include <string_view>
#include <utility>

#include "Image.h"



// Private Methods

void Texture::init(MagFilter magFilter) noexcept
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<int>(magFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<int>(magFilter));
}



//	Public Methods

//constructors
Texture::Texture(std::span<const Pixel> data, int width, int height, MagFilter magFilter) noexcept
	: m_width(width), m_height(height)
{
	init(magFilter);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(std::string_view texture, MagFilter magFilter)
{
	init(magFilter);

	Image image{ texture };
	m_width = image.width();
	m_height = image.height();

	glTexImage2D(GL_TEXTURE_2D, 0, image.format(), image.width(), image.height(), 0, image.format(), GL_UNSIGNED_BYTE, image.data());
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(Texture&& other) noexcept
	: m_width(other.m_width), m_height(other.m_height), m_texture(std::exchange(other.m_texture, 0)) {
}

Texture& Texture::operator= (Texture&& other) noexcept
{
	glDeleteTextures(1, &m_texture);

	m_texture = std::exchange(other.m_texture, 0);
	m_width = other.m_width;
	m_height = other.m_width;

	return *this;
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}



//public methods
void Texture::bind() const noexcept
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::update(std::span<const Pixel> data) const noexcept
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
}