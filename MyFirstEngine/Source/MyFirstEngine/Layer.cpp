#include "Mfepch.h"
#include "Layer.h"

namespace MyFirstEngine
{
	Layer::Layer(const std::string& name)
	: m_LayerName(name)
	{
		MFE_PROFILE_FUNCTION();
	}

	Layer::~Layer()
	{
	}
}