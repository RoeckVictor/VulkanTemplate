#pragma once

namespace MyFirstEngine
{
	class Texture
	{
	public:
		static std::shared_ptr<Texture> CreateFromFile(const std::string& filename);
	};
}