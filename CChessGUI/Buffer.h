#pragma once

#include <GL/glew.h>
#include <span>



class Buffer
{
public:

	//	Public Definitions

	struct Vertex
	{
		float x, y, u, v;
	};



private:

	//	Private Members

	GLuint m_buffer{};
	GLuint m_vao{};
	std::size_t m_capacity{};
	int m_vertexCount{};



private:

	//	Private Methods

	void init() noexcept;

	void cleanup() const noexcept;



public:

	//	Public Methods

	//constructors
	Buffer(std::span<const Vertex> data) noexcept;

	Buffer(const Buffer& other) = delete;

	Buffer(Buffer&& other) noexcept;

	Buffer& operator= (const Buffer& other) = delete;

	Buffer& operator= (Buffer&& other) noexcept;

	~Buffer();



	//getters
	int size() const noexcept;



	//setters
	void buffer(std::span<const Vertex> data) noexcept;

	void bind() const noexcept;
};

