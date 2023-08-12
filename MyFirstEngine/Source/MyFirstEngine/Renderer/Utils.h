#pragma once

namespace MyFirstEngine
{
	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void HashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(HashCombine(seed, rest), ...);
	};

	template <typename T>
	std::vector<uint8_t> ConvertToBytes(const T& value) 
	{
		const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
		return std::vector<uint8_t>(bytes, bytes + sizeof(T));
	}

	template <typename T>
	T ConvertFromBytes(const std::vector<uint8_t>& bytes) 
	{
		if (bytes.size() != sizeof(T)) {
			MFE_CORE_ERROR("Cannot convert bytes to type {0} because the size of the bytes is not the same as the size of the type", typeid(T).name());
			return T{};
		}

		T value;
		std::memcpy(&value, bytes.data(), sizeof(T));
		return value;
	}
}