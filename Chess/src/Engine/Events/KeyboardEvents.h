#pragma once

#include "Events.h"

namespace Engine
{
	class KeyboardEvent : public Event
	{
	public:
		unsigned int GetKey() const { return m_Key; }

	protected:
		KeyboardEvent(unsigned int key) : m_Key(key) {}

		unsigned int m_Key;
	};

	class KeyPressedEvent : public KeyboardEvent
	{
	public:
		KeyPressedEvent(int key) : KeyboardEvent(key) {}

		static EventType GetStaticType() { return EventType::KeyPressed; }

		EventType GetEventType() const override {
			return KeyPressedEvent::GetStaticType();
		}

		const char *GetName() const override { return "KeyPressedEvent"; }
	};

	class KeyReleasedEvent : public KeyboardEvent
	{
	public:
		KeyReleasedEvent(int key) : KeyboardEvent(key) {}

		static EventType GetStaticType() { return EventType::KeyReleased; }

		EventType GetEventType() const override {
			return KeyReleasedEvent::GetStaticType();
		}

		const char *GetName() const override { return "KeyReleasedEvent"; }
	};
} // namespace Engine