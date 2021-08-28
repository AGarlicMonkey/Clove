#include <Clove/Log/Log.hpp>

namespace clove {
    template<typename SubSystemType, typename... Args>
    void Application::pushSubSystem(Args &&...args) {
        pushSubSystem<SubSystemType>(SubSystemGroup::Core, std::forward<Args>(args)...);
    }

    template<typename SubSystemType, typename... Args>
    void Application::pushSubSystem(SubSystemGroup group, Args &&...args) {
        static_assert(std::is_base_of_v<SubSystem, SubSystemType>, "SubSystem provided is not derived from SubSystem.");

        std::type_index const subSystemIndex{ typeid(SubSystemType) };
        CLOVE_ASSERT(subSystemToIndex.find(subSystemIndex) == subSystemToIndex.end(), "Only one subsystem can be active at a time.");

        auto subSystem{ std::make_unique<SubSystemType>(std::forward<Args>(args)...) };

        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "Attached sub system: {0}", subSystem->getName());

        subSystem->onAttach();
        subSystems[group].push_back(std::move(subSystem));

        subSystemToIndex[subSystemIndex] = std::make_pair(group, subSystems[group].size() - 1);
    }

    template<typename SubSystemType>
    SubSystemType &Application::getSubSystem() {
        std::type_index const subSystemIndex{ typeid(SubSystemType) };

        CLOVE_ASSERT(subSystemToIndex.find(subSystemIndex) != subSystemToIndex.end(), "{0}: No subsystem of type provided is currently attached.", CLOVE_FUNCTION_NAME_PRETTY);

        auto &&[group, index] = subSystemToIndex.at(subSystemIndex);
        return *static_cast<SubSystemType *>(subSystems[group][index].get());
    }

    template<typename SubSystemType>
    void Application::popSubSystem() {
        std::type_index const subSystemIndex{ typeid(SubSystemType) };

        if(subSystemToIndex.find(subSystemIndex) == subSystemToIndex.end()) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Warning, "{0}: No subsystem of type provided is currently attached.", CLOVE_FUNCTION_NAME_PRETTY);
            return;
        }

        auto node{ subSystemToIndex.extract(subSystemIndex) };
        auto [group, index] = node.mapped();
        auto &subSystemGroup{ subSystems[group] };

        subSystemGroup[index]->onDetach();
        if(index < subSystemGroup.size() - 1) {
            subSystemGroup[index] = std::move(subSystemGroup.back());

            std::type_index const movedSubSystemIndex{ typeid(subSystemGroup[index]) };
            subSystemToIndex.at(movedSubSystemIndex).second = index;
        }
        subSystemGroup.pop_back();
    }

    Application::State Application::getState() const {
        return currentState;
    }

    Surface *Application::getSurface() const {
        return surface.get();
    }

    GhaDevice *Application::getGraphicsDevice() const {
        return graphicsDevice.get();
    }

    AhaDevice *Application::getAudioDevice() const {
        return audioDevice.get();
    }

    ForwardRenderer3D *Application::getRenderer() const {
        return renderer.get();
    }

    EntityManager *Application::getEntityManager() {
        return &entityManager;
    }

    AssetManager *Application::getAssetManager() {
        return &assetManager;
    }

    VirtualFileSystem *Application::getFileSystem() {
        return &fileSystem;
    }
}