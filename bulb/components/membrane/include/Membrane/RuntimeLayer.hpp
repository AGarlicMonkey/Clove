#pragma once

#include "Membrane/Scene.hpp"

#include <Clove/Layer.hpp>

namespace clove {
    class EntityManager;
}

namespace membrane {
    /**
     * @brief The layer that is active while the game is running.
     * Deliberately does not handle editor events to simulate the game running.
     */
    class RuntimeLayer : public clove::Layer {
        //VARIABLES
    private:
        Scene currentScene;

        //FUNCTIONS
    public:
        RuntimeLayer();

        void onAttach() override;
        void onUpdate(clove::DeltaTime const deltaTime) override;
        void onDetach() override;
    };
}