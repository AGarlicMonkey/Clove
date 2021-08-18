#pragma once

#include "Clove/Layer.hpp"

namespace clove {
    class EntityManager;
}

namespace clove {
    /**
     * @brief Calculates the world matrices of the entities.
     */
    class TransformLayer : public Layer {
        //VARIABLES
    private:
        EntityManager *entityManager{ nullptr };

        //FUNCTIONS
    public:
        TransformLayer() = delete;
		TransformLayer(EntityManager *entityManager);

        TransformLayer(TransformLayer const &other) = delete;
        TransformLayer(TransformLayer &&other) noexcept;

        TransformLayer &operator=(TransformLayer const &other) = delete;
        TransformLayer &operator=(TransformLayer &&other) noexcept;

        ~TransformLayer();

        void onUpdate(DeltaTime const deltaTime) override;
    };
}
