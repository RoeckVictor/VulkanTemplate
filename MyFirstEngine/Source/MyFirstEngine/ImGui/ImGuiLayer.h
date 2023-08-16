#pragma once

#include "MyFirstEngine/Layer.h"
#include "MyFirstEngine/Events/ApplicationEvent.h"
#include "MyFirstEngine/Events/KeyEvent.h"
#include "MyFirstEngine/Events/MouseEvent.h"

namespace MyFirstEngine
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnDetach() override;
		virtual void OnAttach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
		void SetImguiStyle();
	};
}
