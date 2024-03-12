#pragma once

#include <queue>
#include <memory>

#include "Events/Events.h"

namespace Engine {
    class Layer {
    public:
        Layer()=default;

        virtual ~Layer()=default;

        virtual void OnAttach() = 0;

        virtual void OnDetach() = 0;

        virtual bool OnEvent(Event &e) { return false; }
    };

    class LayerStack {
    public:
        LayerStack();

        void Push(Layer* layer);

        void PopBack();
        void PopFront();

        void Remove(int index);

        void OnEvent(Event &e);

    private:
        std::deque<Layer*> m_LayerStack;
    };
}