#pragma once

#include <Clove/SubSystem.hpp>
#include <vector>

namespace clove {
    class EntityManager;
    namespace reflection{
        class TypeInfo;
    }
}

namespace membrane {
    /**
     * @brief Sets up the editor environment to simulate the game's runtime
     */
    class RuntimeSubSystem : public clove::SubSystem {
        //VARIABLES
    private:
        clove::EntityManager *entityManager{ nullptr };

        std::vector<clove::reflection::TypeInfo const *> trackedSubSystems{};

        //FUNCTIONS
    public:
        RuntimeSubSystem() = delete;
        RuntimeSubSystem(clove::EntityManager *manager);

        RuntimeSubSystem(RuntimeSubSystem const &other) = delete;
        RuntimeSubSystem(RuntimeSubSystem &&other) noexcept;

        RuntimeSubSystem &operator=(RuntimeSubSystem const &other) = delete;
        RuntimeSubSystem &operator=(RuntimeSubSystem &&other) noexcept;

        ~RuntimeSubSystem();

        Group getGroup() const override;

        void onAttach() override;
        clove::InputResponse onInputEvent(clove::InputEvent const &inputEvent) override;
        void onUpdate(clove::DeltaTime const deltaTime) override;
        void onDetach() override;
    };
}