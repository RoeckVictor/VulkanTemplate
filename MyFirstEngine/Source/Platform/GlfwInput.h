#pragma once

#include "MyFirstEngine/Application.h"
#include "MyFirstEngine/Input.h"

namespace MyFirstEngine
{
	class GlfwInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}

