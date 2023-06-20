#pragma once

#include "MyFirstEngine/Core.h"

namespace MyFirstEngine 
{
	enum class EventType
	{
		None = 0,
		WindowClose, 
		WindowResize,
		WindowFocus,
		WindowLostFocus, 
		WindowMoved,
		AppTick, 
		AppUpdate, 
		AppRender,
		KeyPressed, 
		KeyReleased,
		KeyTyped,
		MouseButtonPressed,
		MouseButtonReleased, 
		MouseMoved,
		MouseScrolled
	};
	
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

	class Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }

		bool isHandeld = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event) : event(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (event.GetEventType() == T::GetStaticType())
			{
				event.isHandeld = func(*(T*)&event);
				return true;
			}
			return false;
		}

	private:
		Event& event;
	};
	
	inline std::ostream& operator<<(std::ostream& os, const Event& e) { return os << e.ToString(); }
}