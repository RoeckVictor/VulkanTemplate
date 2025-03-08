#include "Mfepch.h"
#include "LayerStack.h"

namespace MyFirstEngine
{
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		MFE_PROFILE_FUNCTION();
		for (Layer* layer : m_Layers)
			delete layer;
	}
	void LayerStack::PushLayer(Layer* layer)
	{
		MFE_PROFILE_FUNCTION();
		m_Layers.emplace(m_Layers.begin()+m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		MFE_PROFILE_FUNCTION();
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		MFE_PROFILE_FUNCTION();
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		MFE_PROFILE_FUNCTION();
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
			m_Layers.erase(it);
	}
}