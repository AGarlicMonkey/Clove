#pragma once

#include "Clove/Layer.hpp"

#include <Clove/DeltaTime.hpp>

namespace clove {
    class ForwardRenderer3D;
    class EntityManager;
}

namespace clove {
    class RenderLayer : public Layer {
        //VARIABLES
    private:
        ForwardRenderer3D *renderer{ nullptr };
        EntityManager *entityManager{ nullptr };

        //FUNCTIONS
    public:
		RenderLayer() = delete;
        RenderLayer(ForwardRenderer3D *renderer, EntityManager *entityManager);

        RenderLayer(RenderLayer const &other) = delete;
        RenderLayer(RenderLayer &&other) noexcept;

        RenderLayer &operator=(RenderLayer const &other) = delete;
        RenderLayer &operator=(RenderLayer &&other) noexcept;

        ~RenderLayer();

        void onUpdate(DeltaTime const deltaTime) override;
    };
}
