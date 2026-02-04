#include "Buffer.h"

#include <cstddef>
#include <GL/glew.h>
#include <span>
#include <utility>
#include <array>

#include "PieceSprite.h"



//	Private Methods

void Buffer::init() noexcept
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, u)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Buffer::cleanup() const noexcept
{
	glDeleteBuffers(1, &m_buffer);
	glDeleteBuffers(1, &m_ebo);
	glDeleteVertexArrays(1, &m_vao);
}



//	Public Methods

//constructors
Buffer::Buffer() noexcept
{
	init();
}

Buffer::Buffer(std::span<const Vertex> data, std::span<const Triangle> indexBuffer) noexcept
{
	init();
	buffer(data, indexBuffer);
}

Buffer Buffer::square(float width) noexcept
{
	static constexpr std::array<Triangle, 2> indexBuffer{
		Triangle(0, 1, 2),
		Triangle(0, 2, 3)
	};

	const float offset{ width * 0.5f };

	const std::array<Vertex, 4> data{
		Vertex(-offset, -offset,   0.0f,   0.0f),
		Vertex( offset, -offset, offset,   0.0f),
		Vertex( offset,  offset, offset, offset),
		Vertex( -offset,  offset, 0.0f, offset),
	};

	return Buffer(data, indexBuffer);
}

Buffer::Buffer(Buffer&& other) noexcept :
	m_buffer(std::exchange(other.m_buffer, 0)),
	m_ebo(std::exchange(other.m_ebo, 0)),
	m_vao(std::exchange(other.m_vao, 0)),
	m_capacity(other.m_capacity),
	m_indexCount(other.m_indexCount) { }

Buffer& Buffer::operator= (Buffer&& other) noexcept
{
	cleanup();

	m_buffer = std::exchange(other.m_buffer, 0);
	m_ebo = std::exchange(other.m_ebo, 0);
	m_vao = std::exchange(other.m_vao, 0);
	m_capacity = other.m_capacity;
	m_indexCount = other.m_indexCount;

	return *this;
}

Buffer::~Buffer()
{
	cleanup();
}



//setters //TODO: maybe make template?
void Buffer::buffer(std::span<const Vertex> data, std::span<const Triangle> indexBuffer) noexcept
{
	static constexpr std::size_t triangleVertexCount{ 3 };

	m_indexCount = static_cast<int>(indexBuffer.size() * triangleVertexCount);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	if (indexBuffer.size_bytes() <= m_capacity)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.size_bytes(), data.data());
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexBuffer.size_bytes(), indexBuffer.data());
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size_bytes(), indexBuffer.data(), GL_STATIC_DRAW);

		m_capacity = indexBuffer.size_bytes();
	}
}

void Buffer::buffer(std::span<const PieceSprite> data, std::span<const Triangle> indexBuffer) noexcept
{
	static constexpr std::size_t triangleVertexCount{ 3 };

	m_indexCount = static_cast<int>(indexBuffer.size() * triangleVertexCount);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	if (indexBuffer.size_bytes() <= m_capacity)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.size_bytes(), data.data());
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexBuffer.size_bytes(), indexBuffer.data());
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size_bytes(), indexBuffer.data(), GL_STATIC_DRAW);

		m_capacity = indexBuffer.size_bytes();
	}
}



//buffer
void Buffer::bind() const noexcept
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
}

void Buffer::draw() const noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBindVertexArray(m_vao);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}