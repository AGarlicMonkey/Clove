#include "Membrane/Scene.hpp"

#include <Clove/Components/PointLightComponent.hpp>
#include <Clove/Components/StaticModelComponent.hpp>
#include <Clove/Components/TransformComponent.hpp>
#include <Clove/ModelLoader.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>
#include <fstream>
#include <typeinfo>

using namespace garlic::clove;

namespace garlic::membrane {
    Scene::Scene(EntityManager *manager, std::filesystem::path saveData)
        : manager{ manager }
        , sceneFile{ std::move(saveData) } {
    }

    Scene::~Scene() = default;

    void Scene::save() {
        serialiser::Node rootNode{};
        for(Entity entity : knownEntities) {
            serialiser::Node entityNode{};
            entityNode["id"]   = entity;
            entityNode["name"] = "Unkown here - needs component?";

            //TODO: Tracking components manually - how to do this more programatically?
            if(manager->hasComponent<TransformComponent>(entity)) {
                auto const &comp{ manager->getComponent<TransformComponent>(entity) };

                serialiser::Node componentNode{ serialise(comp) };
                componentNode["id"] = typeid(comp).hash_code();
                entityNode["components"].pushBack(componentNode);
            }
            if(manager->hasComponent<StaticModelComponent>(entity)) {
                auto &comp{ manager->getComponent<StaticModelComponent>(entity) };

                serialiser::Node componentNode{ serialise(comp) };
                componentNode["id"] = typeid(comp).hash_code();
                entityNode["components"].pushBack(componentNode);
            }
            if(manager->hasComponent<PointLightComponent>(entity)) {
                auto &comp{ manager->getComponent<PointLightComponent>(entity) };

                serialiser::Node componentNode{ serialise(comp) };
                componentNode["id"] = typeid(comp).hash_code();
                entityNode["components"].pushBack(componentNode);
            }

            rootNode["entities"].pushBack(entityNode);
        }

        std::ofstream fileStream{ sceneFile, std::ios::out | std::ios::trunc };
        fileStream << emittYaml(rootNode);
    }

    void Scene::load() {
        //Only destroy known entities to avoid deleting meta ones (such as the camera)
        for(Entity entity : knownEntities) {
            manager->destroy(entity);
        }
        knownEntities.clear();

        auto loadResult{ loadYaml(sceneFile) };
        serialiser::Node rootNode{ loadResult.getValue() };

        for(auto const &entityNode : rootNode["entities"]) {
            Entity entity{ manager->create() };
            for(auto const &componentNode : entityNode["components"]) {
                if(componentNode["id"].as<size_t>() == typeid(TransformComponent).hash_code()) {
                    auto &comp{ manager->addComponent<TransformComponent>(entity, componentNode.as<TransformComponent>()) };
                }
                if(componentNode["id"].as<size_t>() == typeid(StaticModelComponent).hash_code()) {
                    auto &comp{ manager->addComponent<StaticModelComponent>(entity, componentNode.as<StaticModelComponent>()) };
                }
                if(componentNode["id"].as<size_t>() == typeid(PointLightComponent).hash_code()) {
                    auto &comp{ manager->addComponent<PointLightComponent>(entity, componentNode.as<PointLightComponent>()) };
                }
            }

            knownEntities.push_back(entity);
        }
    }

    Entity Scene::createEntity(std::string_view name) {
        Entity entity{ manager->create() };
        knownEntities.push_back(entity);
        return entity;
    }
}