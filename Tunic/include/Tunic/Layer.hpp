#pragma once

namespace tnc{
	class Layer{
		//VARIABLES
	protected:
		std::string debugName;

		//FUNCTIONS
	public:
		Layer(std::string name = "Layer");
		virtual ~Layer() = default;

		virtual void onAttach(){}
		virtual void onUpdate(clv::utl::DeltaTime deltaTime){}
		virtual void onDetach(){}

		const std::string& getName() const;
	};
}