#include <iostream>
#include "Layer.h"

namespace Engine {

	/////////////////////////////// Layer Stack ////////////////////////////////

	LayerStack::LayerStack() = default;

	void LayerStack::Push(Layer *layer) {
		m_LayerStack.push_front(std::move(layer));
		m_LayerStack.front()->OnAttach();
	}

	void LayerStack::PopBack() {
		m_LayerStack.back()->OnDetach();
		m_LayerStack.pop_back();
	}

	void LayerStack::PopFront() {
		m_LayerStack.front()->OnDetach();
		m_LayerStack.pop_front();
	}

	void LayerStack::Remove(int index) {
		std::remove(
				m_LayerStack.begin(), m_LayerStack.end(), m_LayerStack.at
						                                                      (index));
	}

	void LayerStack::OnEvent(Event &e) {
		for (
			Layer *layer: m_LayerStack
				)
			if (layer->OnEvent(e))
				break;
	}

}