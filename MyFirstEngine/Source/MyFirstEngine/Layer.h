#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace MyFirstEngine
{
	class MFE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}

		inline const std::string& GetName() const { return layerName; }

	protected:
		std::string layerName;
	};
}