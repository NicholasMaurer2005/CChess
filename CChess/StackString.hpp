#pragma once

#include <array>
#include <span>
#include <string_view>



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

	std::string_view view() const noexcept
	{
		return m_data;
	}

	std::span<char> get() noexcept
	{
		return m_data;
	}

	const char* data() const noexcept
	{
		return m_data.data();
	}
};