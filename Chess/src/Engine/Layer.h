#pragma once

#include <queue>

#include "Events/Events.h"

namespace Engine {
    class Layer {
    public:
        Layer();

        virtual void OnAttach() = 0;

        virtual void OnDetach() = 0;

        virtual void OnEvent(Event &e) {}
    };

    class LayerStack {
    public:
        LayerStack();

        void Push(Layer *layer);

        void PopBack();

        void Remove(int index);

        void OnEvent(Event &e);

    private:
        std::deque<Layer *> m_LayerStack;
    };
}