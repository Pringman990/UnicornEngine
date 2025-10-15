#pragma once
#include <StandardTypes/StandardTypes.h>
#include <sstream>
#include <iomanip>

class UniqueID128
{
public:
	constexpr UniqueID128() : bytes(0) {};
	constexpr UniqueID128(std::string_view Str) : bytes(FromString(Str)) {};
	constexpr UniqueID128(const UniqueID128&) noexcept = default;
	constexpr UniqueID128(UniqueID128&&) noexcept = default;
	~UniqueID128() = default;

	constexpr bool IsValid() const 
	{ 
		return std::any_of(bytes.begin(), bytes.end(),
			[](byte b) {return b != 0;});
	}

	String ToString() const
	{
		std::ostringstream oss;
		oss << std::hex << std::setfill('0');

		for (size_t i = 0; i < bytes.size(); ++i)
		{
			oss << std::setw(2) << static_cast<int>(bytes[i]);
			if (i == 3 || i == 5 || i == 7 || i == 9) 
				oss << '-';
		}

		return oss.str();
	}

	constexpr const Array<byte, 16>& Bytes() const noexcept { return bytes; }

public:
	constexpr UniqueID128& operator=(const UniqueID128&) noexcept = default;
	constexpr UniqueID128& operator=(UniqueID128&&) noexcept = default;

	constexpr bool operator==(const UniqueID128& other) const noexcept
	{
		return this->bytes == other.bytes;
	}

	constexpr bool operator<(const UniqueID128& other) const noexcept
	{
		return this->bytes < other.bytes;
	}

	constexpr bool operator!=(const UniqueID128& other) const noexcept
	{
		return this->bytes != other.bytes;
	}
private:
	static constexpr Array<byte, 16> FromString(std::string_view Str)
	{
		if (Str.size() != 36)
			throw "UUID string must be 36 chars";

		Array<byte, 16> bytes{};
		size_t bi = 0;

		for (size_t i = 0; i < Str.size(); )
		{
			if (Str[i] == '-') 
			{ 
				++i; 
				continue;
			}

			if (i + 1 >= Str.size() || bi >= 16)
				throw "Malformed UUID";

			auto hex_to_byte = [](char h, char l)->uint8_t 
				{
					auto val = [](char c)->uint8_t
						{
							if ('0' <= c && c <= '9') 
								return c - '0';

							if ('a' <= c && c <= 'f')
								return c - 'a' + 10;

							if ('A' <= c && c <= 'F')
								return c - 'A' + 10;

							throw "Invalid hex digit";
						};
					return static_cast<uint8_t>((val(h) << 4) | val(l));
				};

			bytes[bi++] = hex_to_byte(Str[i], Str[i + 1]);
			i += 2;
		}

		if (bi != 16)
			throw "UUID string incomplete";

		return bytes;
	}
private:
	Array<byte, 16> bytes;
};

// Hash specialization
namespace std 
{
	template<>
	struct hash<UniqueID128>
	{
		size_t operator()(const UniqueID128& id) const noexcept 
		{
			size_t h = 14695981039346656037ULL;

			for (auto b : id.Bytes())
				h = (h ^ b) * 1099511628211ULL;

			return h;
		}
	};
}