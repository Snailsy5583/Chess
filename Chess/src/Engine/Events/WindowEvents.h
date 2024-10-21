#pragma once

#include "Events.h"

namespace Engine
{


	class WindowClosedEvent : public Event
	{
	public:
		WindowClosedEvent() = default;

		static EventType GetStaticType() { return EventType::WindowClosed; }

		EventType GetEventType() const override {
			return WindowClosedEvent::GetStaticType();
		}

		const char *GetName() const override { return "WindowClosed"; }
	};

	class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(int width, int height)
			: m_Width(width), m_Height(height) {}

		static EventType GetStaticType() { return EventType::WindowResized; }

		EventType GetEventType() const override {
			return WindowResizedEvent::GetStaticType();
		}

		const char *GetName() const override { return "WindowResized"; }

		int GetWidth() const { return m_Width; }

		int GetHeight() const { return m_Height; }

	private:
		int m_Width, m_Height;
	};


} // namespace Engine