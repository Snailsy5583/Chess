#pragma once

#include "Events.h"

namespace Engine
{
	class MouseButtonEvent : public Event
	{
	public:
		void GetMousePosition(float &outX, float &outY) const {
			outX = m_MousePosX;
			outY = m_MousePosY;
		}

	protected:
		MouseButtonEvent(int button, float mouseX, float mouseY)
			: m_Button(button), m_MousePosX(mouseX), m_MousePosY(mouseY) {}

		int m_Button;
		float m_MousePosX, m_MousePosY;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button, float mouseX, float mouseY)
			: MouseButtonEvent(button, mouseX, mouseY) {}

		static EventType GetStaticType() {
			return EventType::MouseButtonPressed;
		}

		EventType GetEventType() const override {
			return MouseButtonPressedEvent::GetStaticType();
		}

		const char *GetName() const override { return "MouseButtonPressed"; }
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button, float mouseX, float mouseY)
			: MouseButtonEvent(button, mouseX, mouseY) {}

		static EventType GetStaticType() {
			return EventType::MouseButtonReleased;
		}

		EventType GetEventType() const override {
			return MouseButtonReleasedEvent::GetStaticType();
		}

		const char *GetName() const override { return "MouseButtonReleased"; }
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float mouseX, float mouseY)
			: m_MousePosX(mouseX), m_MousePosY(mouseY) {}

		static EventType GetStaticType() { return EventType::MouseMoved; }

		EventType GetEventType() const override {
			return MouseMovedEvent::GetStaticType();
		}

		const char *GetName() const override { return "MouseMoved"; }

		void GetMousePosition(float &outX, float &outY) const {
			outX = m_MousePosX;
			outY = m_MousePosY;
		}

	private:
		float m_MousePosX, m_MousePosY;
	};

} // namespace Engine