#pragma once

#include <functional>
#include <vector>

namespace Engine
{
#define BIND_EVENT_FUNC(func, obj) std::bind(&func, obj, std::placeholders::_1)

	enum EventType {
		WindowClosed,
		WindowResized,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		KeyPressed,
		KeyReleased
	};

	class Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType GetEventType() const = 0;

		virtual const char *GetName() const = 0;

		inline bool GetIsHandled() const { return m_Handled; }

		inline void SetIsHandled(bool value) { m_Handled = value; }

	protected:
		bool m_Handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFunc = std::function<bool(T &)>;

	public:
		explicit EventDispatcher(Event &event) : m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFunc<T> func) {
			if (m_Event.GetEventType() == T::GetStaticType()) {
				m_Event.m_Handled = func(*(T *) &m_Event);
				return true;
			}
			return false;
		}

	private:
		Event &m_Event;
	};


} // namespace Engine
