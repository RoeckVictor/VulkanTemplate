#include "Mfepch.h"
#include "GameObject.h"
#include "Utils.h"

namespace MyFirstEngine
{
	glm::mat4 TransformComponent::transform() const
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		return glm::mat4
		{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f}
		};
	}

	glm::mat3 TransformComponent::normalMatrix() const
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 invScale = 1.0f / scale;

		return glm::mat3
		{
			{
				invScale.x * (c1 * c3 + s1 * s2 * s3),
				invScale.x * (c2 * s3),
				invScale.x * (c1 * s2 * s3 - c3 * s1),
			},
			{
				invScale.y * (c3 * s1 * s2 - c1 * s3),
				invScale.y * (c2 * c3),
				invScale.y * (c1 * c3 * s2 + s1 * s3),
			},
			{
				invScale.z * (c2 * s1),
				invScale.z * (-s2),
				invScale.z * (c1 * c2)
			}
		};
	}

	void GameObject::Render() const
	{
		m_Material->Bind();
		m_Model->Bind();
		m_Model->Draw();
	}

	GameObject GameObject::CreateGameObject()
	{
		static unsigned int currentId = 0;
		return GameObject(currentId++);
	}

	GameObject GameObject::MakePointLight(float intensity, float radius, glm::vec3 color)
	{
		GameObject gameObj = CreateGameObject();

		gameObj.m_Color = color;
		gameObj.m_Transform.scale.x = radius;
		gameObj.m_PointLight = std::make_unique<PointLightComponent>();
		gameObj.m_PointLight->lightIntensity = intensity;

		VertexArray vertices = VertexArray(VertexLayout({}));
		vertices.count = 6;
		std::shared_ptr<Model> model = Model::CreateModelFromData(vertices, std::vector<uint32_t>());
		gameObj.m_Model = model;

		const std::vector<std::string> shaderFiles = { "../Resources/Shaders/billboard.vert.spv", "../Resources/Shaders/billboard.frag.spv" };
		std::shared_ptr<MyFirstEngine::Shader> shader = Shader::CreateShaderFromCompiledFiles(shaderFiles);
		gameObj.m_Material = Material::CreateMatFromShader(shader);
		gameObj.m_Material->AddUniform(0, "position", ConvertToBytes(glm::vec4(gameObj.m_Transform.translation, 1.0)), true);
		gameObj.m_Material->AddUniform(1, "color", ConvertToBytes(glm::vec4(gameObj.m_Color, intensity)), true);
		gameObj.m_Material->AddUniform(2, "radius", ConvertToBytes(radius), true);
		gameObj.m_Material->AddTexture(0, "AlbedoMap", Texture::CreateFromFile("../Resources/Textures/PointLight.png"));
		gameObj.m_Material->CreatePipeline(vertices);

		return gameObj;
	}
}