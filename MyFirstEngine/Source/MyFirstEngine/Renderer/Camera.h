#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace MyFirstEngine
{
	class Camera
	{
	public:
		void SetOrthographicProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane);
		void SetPerspectiveProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
		void SetViewMatrix(const glm::mat4& viewMatrix) { this->viewMatrix = viewMatrix; };
		void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});
		void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

		glm::mat4 GetProjectionMatrix() const { return projectionMatrix; };
		glm::mat4 GetViewMatrix() const { return viewMatrix; };
		glm::mat4 GetInverseViewMatrix() const { return inverseViewMatrix; };

	private:
		glm::mat4 projectionMatrix{ 1.0f };
		glm::mat4 viewMatrix{ 1.0f };
		glm::mat4 inverseViewMatrix{ 1.0f };
	};
}