#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "MyFirstEngine/Renderer/Model.h"

#include "Device.h"
#include "Buffer.h"
#include "VulkanTexture.h"
#include "Descriptors.h"
#include "VulkanVertex.h"

namespace MyFirstEngine 
{
	class VulkanModel : public Model
	{
	public:
		struct VulkanModelData
		{
			VulkanVertexArray vertices;
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string& filepath);
		};

		VulkanModel(Device& device, const VulkanModelData& builder);
		~VulkanModel();
		VulkanModel(const VulkanModel&) = delete;
		VulkanModel& operator=(const VulkanModel&) = delete;

		void Bind() const override;
		void Draw() const override;

		static std::unique_ptr<Model> CreateModelFromFile(const std::string& filepath, const VertexLayout layout);
		static std::unique_ptr<Model> CreateModelFromData(const VertexArray vertices, const std::vector<uint32_t>& indices);

	private:
		void CreateVertexBuffer(const VulkanVertexArray vertices);
		void CreateIndexBuffer(const std::vector<uint32_t>& indices);

		Device& m_Device;
		std::unique_ptr<Buffer> m_VertexBuffer;
		std::unique_ptr<Buffer> m_IndexBuffer;
	};
}