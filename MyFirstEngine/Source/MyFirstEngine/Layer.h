#pragma once

#include "Core.h"
#include "Engine/Timestep.h"
#include "Events/Event.h"

namespace MyFirstEngine
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep timeStep) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}

		inline const std::string& GetName() const { return m_LayerName; }

	protected:
		std::string m_LayerName;
	};
}