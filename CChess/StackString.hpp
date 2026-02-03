#pragma once

#include <array>
#include <iterator>
#include <string_view>
#include <algorithm>



template<std::size_t Size>
class StackString
{
private:

	//	Private Members

	std::array<char, Size + 1> m_data{};
	std::array<char, Size + 1>::iterator m_back{ m_data.begin() };



public:

	//	Public Members

	//constructors
	StackString() noexcept
	{
		*m_back = '\0';
	}

	StackString(int) noexcept
		: m_back(m_data.end() - 1)
	{
		*m_back = '\0';
	}

	StackString(const StackString& other) noexcept
	{
		std::copy(other.m_data.begin(), other.m_back(), m_data.begin());
		m_back = m_data.begin() + std::distance(other.m_data.begin(), other.m_back);
		*m_back = '\0';
	}

	StackString(StackString&& other) noexcept
	{
		std::copy(other.m_data.begin(), other.m_back(), m_data.begin());
		m_back = m_data.m_begin() + std::distance(other.m_data.begin(), other.m_back);
		*m_back = '\0';
	}

	StackString& operator= (const StackString& other) noexcept
	{
		std::copy(other.m_data.begin(), other.m_back(), m_data.begin());
		m_back = m_data.m_begin() + std::distance(other.m_data.begin(), other.m_back);
		*m_back = '\0';

		return *this;
	}

	StackString& operator= (StackString&& other) noexcept
	{
		std::copy(other.m_data.begin(), other.m_back(), m_data.begin());
		m_back = m_begin() + std::distance(other.m_data.begin(), other.m_back);
		*m_back = '\0';

		return *this;
	}



	//getters
	std::size_t size() const noexcept
	{
		return static_cast<std::size_t>(std::distance(m_data.begin(), m_back));
	}

	std::string_view view() const noexcept
	{
		return std::string_view(m_data.begin(), m_back);
	}

	const char* data() const noexcept
	{
		return m_data.data();
	}



	//setters
	void resize(std::size_t size) noexcept
	{
		m_back = m_data.begin() + size;
		*m_back = '\0';
	}

	char& operator[] (std::size_t index) noexcept
	{
		return m_data[index];
	}



	//string manipulation
	void push(std::string_view data) noexcept
	{
		std::ranges::copy(data, m_back);
		m_back += data.size();
		*m_back = '\0';
	}
};