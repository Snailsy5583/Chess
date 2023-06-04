#include "Layer.h"

namespace Engine {

    Layer::Layer() {

    }

    LayerStack::LayerStack() {}

    void LayerStack::Push(Layer *layer) {
        m_LayerStack.push_front(layer);
        layer->OnAttach();
    }

    void LayerStack::PopBack() {
        m_LayerStack.back()->OnDetach();
        m_LayerStack.pop_back();
    }

    void LayerStack::Remove(int index) {}

    void LayerStack::OnEvent(Event &e) {
        for (Layer *layer: m_LayerStack)
            layer->OnEvent(e);
    }

}