#pragma once

#include <memory>
#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"

namespace VulkanTutorial
{
	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f };
		glm::vec3 rotation{ 0.0f };

		glm::mat4 transform();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponent {
		float lightIntensity = 1.0f;
		// glm::vec3 color{};
	};

	class GameObject
	{
	public:
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		static GameObject CreateGameObject();
		static GameObject MakePointLight(float intensity = 1.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));
		unsigned int GetId() const { return id; }

		
		glm::vec3 color{};
		TransformComponent transform{};

		std::shared_ptr<Model> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;

		

	private:
		GameObject(unsigned int objId) : id(objId) {}

		unsigned int id;
	};
}