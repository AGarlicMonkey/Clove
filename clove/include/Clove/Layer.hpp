#pragma once

#include "Clove/InputResponse.hpp"
#include "Clove/InputEvent.hpp"

#include <Clove/DeltaTime.hpp>

namespace garlic::clove {
    class Layer {
        //VARIABLES
    protected:
        std::string debugName;

        //FUNCTIONS
    public:
        Layer(std::string name = "Layer");
        virtual ~Layer() = default;

        virtual void onAttach() {}
        virtual InputResponse onInputEvent(InputEvent const &inputEvent);
        virtual void onUpdate(DeltaTime const deltaTime) {}
        virtual void onDetach() {}

        inline const std::string &getName() const;
    };
}

#include "Layer.inl"