#pragma once

#include "Clove/SubSystem.hpp"

#include <Clove/DeltaTime.hpp>

namespace clove {
    class Renderer;
    class EntityManager;
}

namespace clove {
    class RenderSubSystem : public SubSystem {
        //VARIABLES
    private:
        Renderer *renderer{ nullptr };
        EntityManager *entityManager{ nullptr };

        //FUNCTIONS
    public:
		RenderSubSystem() = delete;
        RenderSubSystem(Renderer *renderer, EntityManager *entityManager);

        RenderSubSystem(RenderSubSystem const &other) = delete;
        RenderSubSystem(RenderSubSystem &&other) noexcept;

        RenderSubSystem &operator=(RenderSubSystem const &other) = delete;
        RenderSubSystem &operator=(RenderSubSystem &&other) noexcept;

        ~RenderSubSystem();

        Group getGroup() const override;

        void onAttach() override{};
        InputResponse onInputEvent(InputEvent const &inputEvent) override{ return InputResponse::Ignored; }
        void onUpdate(DeltaTime const deltaTime) override;
        void onDetach() override{};
    };
}
