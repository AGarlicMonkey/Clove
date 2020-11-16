#pragma once

#include <Bulb/ECS/System.hpp>

namespace clv::plt {
    class Window;
}

namespace garlic::inline stem {
    class ForwardRenderer3D;
}

namespace garlic::inline stem {
    class RenderSystem : public blb::ecs::System {
        //VARIABLES
    private:
        std::shared_ptr<ForwardRenderer3D> renderer;

        //FUNCTIONS
    public:
        RenderSystem() = delete;
        RenderSystem(std::shared_ptr<ForwardRenderer3D> renderer);

        RenderSystem(RenderSystem const &other) = delete;
        RenderSystem(RenderSystem &&other) noexcept;

        RenderSystem &operator=(RenderSystem const &other) = delete;
        RenderSystem &operator=(RenderSystem &&other) noexcept;

        ~RenderSystem();

        void update(blb::ecs::World &world, clv::utl::DeltaTime deltaTime) override;
    };
}