#pragma once

#include <array>
#include <span>



template<std::size_t Size>
class StackString
{
private:

	std::array<char, Size> m_data{};

public:

	StackString() noexcept
	{
		m_data.back() = '\0';
	}

	const std::span<char> view() noexcept
	{
		return m_data;
	}

	const char* data() const noexcept
	{
		return m_data;
	}
};