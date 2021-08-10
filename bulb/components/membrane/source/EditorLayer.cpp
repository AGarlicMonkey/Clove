#include "Membrane/EditorLayer.hpp"

#include "Membrane/MessageHandler.hpp"
#include "Membrane/Messages.hpp"
#include "Membrane/NameComponent.hpp"

#include <Clove/Application.hpp>
#include <Clove/Components/CameraComponent.hpp>
#include <Clove/Components/CollisionShapeComponent.hpp>
#include <Clove/Components/PointLightComponent.hpp>
#include <Clove/Components/RigidBodyComponent.hpp>
#include <Clove/Components/StaticModelComponent.hpp>
#include <Clove/Components/TransformComponent.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Layers/PhysicsLayer.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Maths/MathsHelpers.hpp>
#include <Clove/ModelLoader.hpp>
#include <Clove/Surface.hpp>
#include <msclr/marshal_cppstd.h>

namespace garlic::membrane {
    // clang-format off
    /**
     * @brief 
     */
    private ref class EditorLayerMessageProxy { //TODO: This will move into editor layer - runtime layer does not respond to editor
        //VARIABLES
    private:
        EditorLayer *layer{ nullptr };

        //FUNCTIONS
    public:
        EditorLayerMessageProxy(EditorLayer *layer)
            : layer{ layer } {
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_CreateEntity ^>(this, &EditorLayerMessageProxy::createEntity));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_DeleteEntity ^>(this, &EditorLayerMessageProxy::deleteEntity));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_CreateComponent ^>(this, &EditorLayerMessageProxy::createComponent));

            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateTransform ^>(this, &EditorLayerMessageProxy::updateTransform));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateStaticModel ^>(this, &EditorLayerMessageProxy::updateStaticModel));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateRigidBody ^>(this, &EditorLayerMessageProxy::updateRigidBody));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateSphereShape ^>(this, &EditorLayerMessageProxy::updateSphereShape));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateCubeShape ^>(this, &EditorLayerMessageProxy::updateCubeShape));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateName ^>(this, &EditorLayerMessageProxy::updateName));

            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_SaveScene ^>(this, &EditorLayerMessageProxy::saveScene));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_LoadScene ^>(this, &EditorLayerMessageProxy::loadScene));
        }

    private:
        void createEntity(Editor_CreateEntity ^ message) {
            layer->createEntity();
        }

        void deleteEntity(Editor_DeleteEntity ^ message) {
            clove::Entity const entity{ message->entity };
            layer->deleteEntity(entity);
        }

        void createComponent(Editor_CreateComponent ^ message){
            layer->createComponent(message->entity, message->componentType);
        }

        void updateTransform(Editor_UpdateTransform ^ message){
            clove::vec3f const pos{message->position.x, message->position.y, message->position.z};
            clove::vec3f const rot{clove::asRadians(message->rotation.x), clove::asRadians(message->rotation.y), clove::asRadians(message->rotation.z)};
            clove::vec3f const scale{message->scale.x, message->scale.y, message->scale.z};

            layer->updateTransform(message->entity, pos, rot, scale);
        }

        void updateStaticModel(Editor_UpdateStaticModel ^ message){
            System::String ^meshPath{ message->meshPath };
            System::String ^diffusePath{ message->diffusePath };
            System::String ^specularPath{ message->specularPath };
            layer->updateStaticModel(message->entity, msclr::interop::marshal_as<std::string>(meshPath), msclr::interop::marshal_as<std::string>(diffusePath), msclr::interop::marshal_as<std::string>(specularPath));
        }

        void updateRigidBody(Editor_UpdateRigidBody^ message){
            layer->updateRigidBody(message->entity, message->mass);
        }

        void updateSphereShape(Editor_UpdateSphereShape ^message){
            layer->updateSphereShape(message->entity, message->radius);
        }

        void updateCubeShape(Editor_UpdateCubeShape ^message){
            clove::vec3f const halfExtents{message->halfExtents.x, message->halfExtents.y, message->halfExtents.z};

            layer->updateCubeShape(message->entity, halfExtents);
        }

        void updateName(Editor_UpdateName ^ message){
            System::String ^name{ message->name };
            layer->updateName(message->entity, msclr::interop::marshal_as<std::string>(name));
        }

        void saveScene(Editor_SaveScene ^message){
            layer->saveScene();
        }

        void loadScene(Editor_LoadScene ^message){
            layer->loadScene();
        }
    };
    // clang-format on
}

namespace garlic::membrane {
    EditorLayer::EditorLayer()
        : clove::Layer{ "Editor Layer" }
        , currentScene{ clove::Application::get().getEntityManager(), "scene.yaml" } {
        proxy = gcnew EditorLayerMessageProxy(this);
    }

    void EditorLayer::onAttach() {
        auto &app{ clove::Application::get() };

        //Pop the physics layer from the application
        app.popLayer(app.getPhysicsLayer());

        //Add the editor camera outside of the current scene
        if(editorCamera == clove::NullEntity) {
            auto *const entityManager{ app.getEntityManager() };

            editorCamera                                                                  = entityManager->create();
            entityManager->addComponent<clove::TransformComponent>(editorCamera).position = clove::vec3f{ 0.0f, 0.0f, -10.0f };
            entityManager->addComponent<clove::CameraComponent>(editorCamera, clove::Camera{ clove::Camera::ProjectionMode::Perspective });
        }

        loadScene();
    }

    clove::InputResponse EditorLayer::onInputEvent(clove::InputEvent const &inputEvent) {
        if(inputEvent.eventType == clove::InputEvent::Type::Mouse) {
            auto event{ std::get<clove::Mouse::Event>(inputEvent.event) };

            if(event.getButton() == clove::MouseButton::Right) {
                if(event.getType() == clove::Mouse::Event::Type::Pressed) {
                    //Make sure prev + pos gets updated when the event happens.
                    //This fixes situations where the mouse will be up, move and then go down
                    mousePos     = event.getPos();
                    prevMousePos = event.getPos();

                    moveMouse = true;
                    return clove::InputResponse::Consumed;
                } else {
                    moveMouse = false;
                    return clove::InputResponse::Consumed;
                }
            }

            if(event.getType() == clove::Mouse::Event::Type::Move && moveMouse) {
                mousePos = event.getPos();
                return clove::InputResponse::Consumed;
            }
        }
        return clove::InputResponse::Ignored;
    }

    void EditorLayer::onUpdate(clove::DeltaTime const deltaTime) {
        //Broadcast up any transform changes
        for(auto &entity : currentScene.getKnownEntities()) {
            if(currentScene.hasComponent<clove::TransformComponent>(entity)) {
                auto const &pos{ currentScene.getComponent<clove::TransformComponent>(entity).position };
                auto const &rot{ clove::quaternionToEuler(currentScene.getComponent<clove::TransformComponent>(entity).rotation) };
                auto const &scale{ currentScene.getComponent<clove::TransformComponent>(entity).scale };

                Engine_OnTransformChanged ^ message { gcnew Engine_OnTransformChanged };
                message->entity   = entity;
                message->position = Vector3(pos.x, pos.y, pos.z);
                message->rotation = Vector3(clove::asDegrees(rot.x), clove::asDegrees(rot.y), clove::asDegrees(rot.z));
                message->scale    = Vector3(scale.x, scale.y, scale.z);
                MessageHandler::sendMessage(message);
            }
        }

        auto &keyBoard{ clove::Application::get().getSurface()->getKeyboard() };
        auto &mouse{ clove::Application::get().getSurface()->getMouse() };
        auto *const entityManager{ clove::Application::get().getEntityManager() };

        clove::vec3f pos{};
        clove::vec3f rot{};

        //Camera Movement: Keyboard
        if(keyBoard.isKeyPressed(clove::Key::W)) {
            pos.z += 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::S)) {
            pos.z -= 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::A)) {
            pos.x -= 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::D)) {
            pos.x += 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::Space) || keyBoard.isKeyPressed(clove::Key::E)) {
            pos.y += 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::Shift_Left) || keyBoard.isKeyPressed(clove::Key::Q)) {
            pos.y -= 1.0f;
        }

        //Camera Movement: Mouse
        if(moveMouse) {
            float constexpr speed = 10.0f;
            clove::vec2i const delta{ mousePos - prevMousePos };

            mouseLookYaw += static_cast<float>(delta.x) * speed * deltaTime;
            mouseLookPitch += static_cast<float>(delta.y) * speed * deltaTime;

            prevMousePos = mousePos;
        }

        rot.x = clove::asRadians(mouseLookPitch);
        rot.y = clove::asRadians(mouseLookYaw);

        auto &camTrans{ entityManager->getComponent<clove::TransformComponent>(editorCamera) };
        camTrans.rotation = rot;
        camTrans.position += camTrans.rotation * pos * 10.0f * deltaTime.getDeltaSeconds();
    }

    void EditorLayer::onDetach() {
        saveScene();
        currentScene.destroyAllEntities();
    }

    void EditorLayer::saveScene() {
        currentScene.save();
    }

    void EditorLayer::loadScene() {
        currentScene.load();

        Engine_OnSceneLoaded ^ loadMessage { gcnew Engine_OnSceneLoaded };
        loadMessage->entities = gcnew System::Collections::Generic::List<Entity ^>{};
        for(auto entity : currentScene.getKnownEntities()) {
            Entity ^ editorEntity { gcnew Entity };
            editorEntity->id         = entity;
            editorEntity->name       = gcnew System::String(currentScene.getComponent<NameComponent>(entity).name.c_str());
            editorEntity->components = gcnew System::Collections::Generic::List<Component ^>{};

            //Add all of the component types for an entity
            if(currentScene.hasComponent<clove::TransformComponent>(entity)) {
                Component ^ componentData { gcnew Component{} };
                componentData->type = ComponentType::Transform;

                editorEntity->components->Add(componentData);
            }
            if(currentScene.hasComponent<clove::StaticModelComponent>(entity)) {
                StaticModelComponentInitData ^ initData { gcnew StaticModelComponentInitData{} };
                initData->meshPath     = gcnew System::String(currentScene.getComponent<clove::StaticModelComponent>(entity).model.getPath().c_str());
                initData->diffusePath  = gcnew System::String(currentScene.getComponent<clove::StaticModelComponent>(entity).material->getDiffuseTexture().getPath().c_str());
                initData->specularPath = gcnew System::String(currentScene.getComponent<clove::StaticModelComponent>(entity).material->getSpecularTexture().getPath().c_str());

                Component ^ componentData = gcnew Component{};
                componentData->type       = ComponentType::StaticModel;
                componentData->initData   = initData;

                editorEntity->components->Add(componentData);
            }
            if(currentScene.hasComponent<clove::PointLightComponent>(entity)) {
                Component ^ componentData = gcnew Component{};
                componentData->type       = ComponentType::PointLight;

                editorEntity->components->Add(componentData);
            }
            if(currentScene.hasComponent<clove::RigidBodyComponent>(entity)) {
                RigidBodyComponentInitData ^ initData = gcnew RigidBodyComponentInitData{};
                initData->mass                        = currentScene.getComponent<clove::RigidBodyComponent>(entity).mass;

                Component ^ componentData = gcnew Component{};
                componentData->type       = ComponentType::RigidBody;
                componentData->initData   = initData;

                editorEntity->components->Add(componentData);
            }
            if(currentScene.hasComponent<clove::CollisionShapeComponent>(entity)) {
                auto &component{ currentScene.getComponent<clove::CollisionShapeComponent>(entity) };

                CollisionShapeComponentInitData ^ initData { gcnew CollisionShapeComponentInitData{} };
                if(auto *sphere{ std::get_if<clove::CollisionShapeComponent::Sphere>(&component.shape) }) {
                    initData->shapeType = ShapeType::Sphere;
                    initData->radius    = sphere->radius;
                } else if(auto *cube{ std::get_if<clove::CollisionShapeComponent::Cube>(&component.shape) }) {
                    initData->shapeType   = ShapeType::Cube;
                    initData->halfExtents = Vector3(cube->halfExtents.x, cube->halfExtents.y, cube->halfExtents.z);
                }

                Component ^ componentData = gcnew Component{};
                componentData->type       = ComponentType::CollisionShape;
                componentData->initData   = initData;

                editorEntity->components->Add(componentData);
            }

            loadMessage->entities->Add(editorEntity);
        }

        MessageHandler::sendMessage(loadMessage);
    }

    clove::Entity EditorLayer::createEntity(std::string_view name) {
        clove::Entity entity{ currentScene.createEntity() };
        currentScene.addComponent<NameComponent>(entity, std::string{ std::move(name) });

        Engine_OnEntityCreated ^ message { gcnew Engine_OnEntityCreated };
        message->entity = entity;
        message->name   = gcnew System::String(std::string{ std::begin(name), std::end(name) }.c_str());
        MessageHandler::sendMessage(message);

        return entity;
    }

    void EditorLayer::deleteEntity(clove::Entity entity) {
        currentScene.deleteEntity(entity);

        Engine_OnEntityDeleted ^ message { gcnew Engine_OnEntityDeleted };
        message->entity = entity;
        MessageHandler::sendMessage(message);
    }

    void EditorLayer::createComponent(clove::Entity entity, ComponentType componentType) {
        bool added{ false };
        System::Object ^ initData;

        switch(componentType) {
            case ComponentType::Transform:
                if(!currentScene.hasComponent<clove::TransformComponent>(entity)) {
                    currentScene.addComponent<clove::TransformComponent>(entity);
                    added = true;
                }
                break;
            case ComponentType::StaticModel:
                if(!currentScene.hasComponent<clove::StaticModelComponent>(entity)) {
                    std::string const meshPath{ "./cube.obj" };//TEMP: Hard coding to a cube for now. This will either need to just be raw vertex data or an internal cube shipped with the editor
                    currentScene.addComponent<clove::StaticModelComponent>(entity, clove::Application::get().getAssetManager()->getStaticModel(meshPath));

                    StaticModelComponentInitData ^ modelData { gcnew StaticModelComponentInitData{} };
                    modelData->meshPath = gcnew System::String(meshPath.c_str());

                    added    = true;
                    initData = modelData;
                }
                break;
            case ComponentType::PointLight:
                if(!currentScene.hasComponent<clove::PointLightComponent>(entity)) {
                    currentScene.addComponent<clove::PointLightComponent>(entity);
                    added = true;
                }
                break;
            case ComponentType::RigidBody:
                if(!currentScene.hasComponent<clove::RigidBodyComponent>(entity)) {
                    currentScene.addComponent<clove::RigidBodyComponent>(entity);

                    RigidBodyComponentInitData ^ bodyData { gcnew RigidBodyComponentInitData{} };
                    bodyData->mass = currentScene.getComponent<clove::RigidBodyComponent>(entity).mass;

                    added    = true;
                    initData = bodyData;
                }
                break;
            case ComponentType::CollisionShape:
                if(!currentScene.hasComponent<clove::CollisionShapeComponent>(entity)) {
                    float constexpr initRadius{ 1.0f };
                    currentScene.addComponent<clove::CollisionShapeComponent>(entity, clove::CollisionShapeComponent::Sphere{ initRadius });

                    CollisionShapeComponentInitData ^ shapeData { gcnew CollisionShapeComponentInitData{} };
                    shapeData->shapeType = ShapeType::Sphere;
                    shapeData->radius    = initRadius;

                    added    = true;
                    initData = shapeData;
                }
                break;
        }

        if(added) {
            Engine_OnComponentCreated ^ message { gcnew Engine_OnComponentCreated };
            message->entity        = entity;
            message->componentType = componentType;
            message->data          = initData;
            MessageHandler::sendMessage(message);
        }
    }

    void EditorLayer::updateTransform(clove::Entity entity, clove::vec3f position, clove::vec3f rotation, clove::vec3f scale) {
        if(currentScene.hasComponent<clove::TransformComponent>(entity)) {
            auto &transform{ currentScene.getComponent<clove::TransformComponent>(entity) };
            transform.position = position;
            transform.rotation = rotation;
            transform.scale    = scale;
        }
    }

    void EditorLayer::updateStaticModel(clove::Entity entity, std::string meshPath, std::string diffusePath, std::string specularPath) {
        if(currentScene.hasComponent<clove::StaticModelComponent>(entity)) {
            auto &staticModel{ currentScene.getComponent<clove::StaticModelComponent>(entity) };
            staticModel.model = clove::Application::get().getAssetManager()->getStaticModel(meshPath);
            if(!diffusePath.empty()) {
                staticModel.material->setDiffuseTexture(clove::Application::get().getAssetManager()->getTexture(diffusePath));
            }
            if(!specularPath.empty()) {
                staticModel.material->setSpecularTexture(clove::Application::get().getAssetManager()->getTexture(specularPath));
            }
        }
    }

    void EditorLayer::updateRigidBody(clove::Entity entity, float mass) {
        if(currentScene.hasComponent<clove::RigidBodyComponent>(entity)) {
            auto &rigidBody{ currentScene.getComponent<clove::RigidBodyComponent>(entity) };
            rigidBody.mass = mass;

            //Pipe back up the change to the editor
            Engine_OnRigidBodyChanged ^ message { gcnew Engine_OnRigidBodyChanged };
            message->entity = entity;
            message->mass   = mass;
            MessageHandler::sendMessage(message);
        }
    }

    void EditorLayer::updateSphereShape(clove::Entity entity, float radius) {
        if(currentScene.hasComponent<clove::CollisionShapeComponent>(entity)) {
            currentScene.getComponent<clove::CollisionShapeComponent>(entity).shape = clove::CollisionShapeComponent::Sphere{ radius };

            Engine_OnSphereShapeChanged ^ message { gcnew Engine_OnSphereShapeChanged };
            message->entity = entity;
            message->radius = radius;
            MessageHandler::sendMessage(message);
        }
    }

    void EditorLayer::updateCubeShape(clove::Entity entity, clove::vec3f halfExtents) {
        if(currentScene.hasComponent<clove::CollisionShapeComponent>(entity)) {
            currentScene.getComponent<clove::CollisionShapeComponent>(entity).shape = clove::CollisionShapeComponent::Cube{ halfExtents };

            Engine_OnCubeShapeChanged ^ message { gcnew Engine_OnCubeShapeChanged };
            message->entity      = entity;
            message->halfExtents = Vector3{ halfExtents.x, halfExtents.y, halfExtents.z };
            MessageHandler::sendMessage(message);
        }
    }

    void EditorLayer::updateName(clove::Entity entity, std::string name) {
        if(currentScene.hasComponent<NameComponent>(entity)) {
            currentScene.getComponent<NameComponent>(entity).name = std::move(name);
        }
    }
}