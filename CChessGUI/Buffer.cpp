#include "Buffer.h"

#include <cstddef>
#include <GL/glew.h>
#include <span>
#include <utility>



//	Private Methods

void Buffer::init() noexcept
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, u)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Buffer::cleanup() const noexcept
{
	glDeleteBuffers(1, &m_buffer);
	glDeleteVertexArrays(1, &m_vao);
}



//	Public Methods

//constructors
Buffer::Buffer(std::span<const Vertex> data) noexcept
{
	init();
	buffer(data);
}

Buffer::Buffer(Buffer&& other) noexcept :
	m_buffer(std::exchange(other.m_buffer, 0)),
	m_vao(std::exchange(other.m_vao, 0)),
	m_capacity(other.m_capacity),
	m_vertexCount(other.m_vertexCount) { }

Buffer& Buffer::operator= (Buffer&& other) noexcept
{
	cleanup();

	m_buffer = std::exchange(other.m_buffer, 0);
	m_vao = std::exchange(other.m_vao, 0);
	m_capacity = other.m_capacity;
	m_vertexCount = other.m_vertexCount;

	return *this;
}

Buffer::~Buffer()
{
	cleanup();
}



//getters
int Buffer::size() const noexcept
{
	return m_vertexCount;
}



//setters
void Buffer::buffer(std::span<const Vertex> data) noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	m_vertexCount = static_cast<int>(data.size());

	if (data.size_bytes() <= m_capacity)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.size_bytes(), data.data());
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STATIC_DRAW);
		m_capacity = data.size_bytes();
	}
}

void Buffer::bind() const noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBindVertexArray(m_vao);
}