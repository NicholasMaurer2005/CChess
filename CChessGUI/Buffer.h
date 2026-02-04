#pragma once

#include <GL/glew.h>
#include <span>

#include "PieceSprite.h"



class Buffer
{
public:

	//	Public Definitions

	struct Vertex
	{
		float x, y, u, v;
	};

	struct Triangle
	{
		GLuint v1, v2, v3;
	};



private:

	//	Private Members

	GLuint m_buffer{};
	GLuint m_ebo{};
	GLuint m_vao{};
	GLuint m_indexBuffer{};
	std::size_t m_capacity{};
	int m_indexCount{};



private:

	//	Private Methods
	void cleanup() const noexcept;



public:

	//	Public Methods

	//constructors
	Buffer() noexcept {};

	Buffer(std::span<const Vertex> data, std::span<const Triangle> indexBuffer) noexcept;

	static Buffer square(float width) noexcept;

	Buffer(const Buffer& other) = delete;

	Buffer(Buffer&& other) noexcept;

	Buffer& operator= (const Buffer& other) = delete;

	Buffer& operator= (Buffer&& other) noexcept;

	~Buffer();



	//setters
	void buffer(std::span<const Vertex> data, std::span<const Triangle> indexBuffer) noexcept;

	void buffer(std::span<const PieceSprite> data, std::span<const Triangle> indexBuffer) noexcept;



	//buffer
	void initialize() noexcept;

	void bind() const noexcept;

	void draw() const noexcept;
};

