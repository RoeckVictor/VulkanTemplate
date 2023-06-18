#pragma once

#include "Mfepch.h"

#include "Core.h"
#include "Events/Event.h"

namespace MyFirstEngine
{
	struct WindowInfo
	{
		std::string title = "My First Engine";
		unsigned int width = 1280;
		unsigned int height = 720;
	};

	class MFE_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowInfo& info = WindowInfo());
	};
}