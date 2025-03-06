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
		void SetViewMatrix(const glm::mat4& viewMatrix) { this->m_ViewMatrix = viewMatrix; };
		void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});
		void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

		glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; };
		glm::mat4 GetViewMatrix() const { return m_ViewMatrix; };
		glm::mat4 GetInverseViewMatrix() const { return m_InverseViewMatrix; };

	private:
		glm::mat4 m_ProjectionMatrix{ 1.0f };
		glm::mat4 m_ViewMatrix{ 1.0f };
		glm::mat4 m_InverseViewMatrix{ 1.0f };
	};
}