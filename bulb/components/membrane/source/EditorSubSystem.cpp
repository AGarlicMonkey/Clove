#include "Membrane/EditorSubSystem.hpp"

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
#include <Clove/Log/Log.hpp>
#include <Clove/Maths/MathsHelpers.hpp>
#include <Clove/ModelLoader.hpp>
#include <Clove/SubSystems/PhysicsSubSystem.hpp>
#include <Clove/Surface.hpp>
#include <msclr/marshal_cppstd.h>

namespace membrane {
    // clang-format off
    /**
     * @brief 
     */
    private ref class EditorSubSystemMessageProxy {
        //VARIABLES
    private:
        EditorSubSystem *subSystem{ nullptr };

        //FUNCTIONS
    public:
        EditorSubSystemMessageProxy(EditorSubSystem *layer)
            : subSystem{ layer } {
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_CreateEntity ^>(this, &EditorSubSystemMessageProxy::createEntity));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_DeleteEntity ^>(this, &EditorSubSystemMessageProxy::deleteEntity));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_AddComponent ^>(this, &EditorSubSystemMessageProxy::addComponent));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_RemoveComponent ^>(this, &EditorSubSystemMessageProxy::removeComponent));

            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateTransform ^>(this, &EditorSubSystemMessageProxy::updateTransform));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateStaticModel ^>(this, &EditorSubSystemMessageProxy::updateStaticModel));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateRigidBody ^>(this, &EditorSubSystemMessageProxy::updateRigidBody));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateSphereShape ^>(this, &EditorSubSystemMessageProxy::updateSphereShape));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateCubeShape ^>(this, &EditorSubSystemMessageProxy::updateCubeShape));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateName ^>(this, &EditorSubSystemMessageProxy::updateName));

            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_SaveScene ^>(this, &EditorSubSystemMessageProxy::saveScene));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_LoadScene ^>(this, &EditorSubSystemMessageProxy::loadScene));
        }

    public:
        void reset(){
            subSystem = nullptr;
        }

    private:
        void createEntity(Editor_CreateEntity ^ message) {
            if (subSystem){
                subSystem->createEntity();
            }
        }

        void deleteEntity(Editor_DeleteEntity ^ message) {
            if (subSystem){
                clove::Entity const entity{ message->entity };
                subSystem->deleteEntity(entity);
            }
        }

        void addComponent(Editor_AddComponent ^ message){
            if (subSystem){
                subSystem->addComponent(message->entity, message->componentType);
            }
        }

        void removeComponent(Editor_RemoveComponent ^ message){
            if (subSystem){
                subSystem->removeComponent(message->entity, message->componentType);
            }
        }

        void updateTransform(Editor_UpdateTransform ^ message){
            if (subSystem){
                clove::vec3f const pos{message->position.x, message->position.y, message->position.z};
                clove::vec3f const rot{clove::asRadians(message->rotation.x), clove::asRadians(message->rotation.y), clove::asRadians(message->rotation.z)};
                clove::vec3f const scale{message->scale.x, message->scale.y, message->scale.z};

                subSystem->updateTransform(message->entity, pos, rot, scale);
            }
        }

        void updateStaticModel(Editor_UpdateStaticModel ^ message){
            if (subSystem){
                System::String ^meshPath{ message->meshPath != nullptr ? message->meshPath : "" };
                System::String ^diffusePath{ message->diffusePath != nullptr ? message->diffusePath : ""};
                System::String ^specularPath{ message->specularPath != nullptr ? message->specularPath : ""};

                subSystem->updateStaticModel(message->entity, msclr::interop::marshal_as<std::string>(meshPath), msclr::interop::marshal_as<std::string>(diffusePath), msclr::interop::marshal_as<std::string>(specularPath));
            }
        }

        void updateRigidBody(Editor_UpdateRigidBody^ message){
            if (subSystem){
                subSystem->updateRigidBody(message->entity, message->mass);
            }
        }

        void updateSphereShape(Editor_UpdateSphereShape ^message){
            if (subSystem){
                subSystem->updateSphereShape(message->entity, message->radius);
            }
        }

        void updateCubeShape(Editor_UpdateCubeShape ^message){
            if (subSystem){
                clove::vec3f const halfExtents{message->halfExtents.x, message->halfExtents.y, message->halfExtents.z};

                subSystem->updateCubeShape(message->entity, halfExtents);
            }
        }

        void updateName(Editor_UpdateName ^ message){
            if (subSystem){
                System::String ^name{ message->name };
                subSystem->updateName(message->entity, msclr::interop::marshal_as<std::string>(name));
            }
        }

        void saveScene(Editor_SaveScene ^message){
            if (subSystem){
                subSystem->saveScene();
            }
        }

        void loadScene(Editor_LoadScene ^message){
            if (subSystem){
                subSystem->loadScene();
            }
        }
    };
    // clang-format on
}

namespace membrane {
    EditorSubSystem::EditorSubSystem()
        : clove::SubSystem{ "Editor SubSystem" }
        , currentScene{ clove::Application::get().getEntityManager() } {
        proxy = gcnew EditorSubSystemMessageProxy(this);
    }

    EditorSubSystem::~EditorSubSystem() {
        proxy->reset();
    }

    void EditorSubSystem::onAttach() {
        auto &app{ clove::Application::get() };

        //Pop the physics layer from the application
        app.popSubSystem<clove::PhysicsSubSystem>();

        //Add the editor camera outside of the current scene
        if(editorCamera == clove::NullEntity) {
            auto *const entityManager{ app.getEntityManager() };

            editorCamera                                                                  = entityManager->create();
            entityManager->addComponent<clove::TransformComponent>(editorCamera).position = clove::vec3f{ 0.0f, 0.0f, -10.0f };
            entityManager->addComponent<clove::CameraComponent>(editorCamera, clove::Camera{ clove::Camera::ProjectionMode::Perspective });
        }

        loadScene();
    }

    clove::InputResponse EditorSubSystem::onInputEvent(clove::InputEvent const &inputEvent) {
        if(auto const *const mouseEvent{ std::get_if<clove::Mouse::Event>(&inputEvent) }) {
            if(mouseEvent->getButton() == clove::MouseButton::Right) {
                if(mouseEvent->getType() == clove::Mouse::Event::Type::Pressed) {
                    //Make sure prev + pos gets updated when the event happens.
                    //This fixes situations where the mouse will be up, move and then go down
                    mousePos     = mouseEvent->getPos();
                    prevMousePos = mouseEvent->getPos();

                    moveMouse = true;
                    return clove::InputResponse::Consumed;
                } else {
                    moveMouse = false;
                    return clove::InputResponse::Consumed;
                }
            }

            if(mouseEvent->getType() == clove::Mouse::Event::Type::Move && moveMouse) {
                mousePos = mouseEvent->getPos();
                return clove::InputResponse::Consumed;
            }
        }
        return clove::InputResponse::Ignored;
    }

    void EditorSubSystem::onUpdate(clove::DeltaTime const deltaTime) {
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

    void EditorSubSystem::onDetach() {
        //saveScene();
        currentScene.destroyAllEntities();
    }

    void EditorSubSystem::saveScene() {
        currentScene.save(clove::Application::get().getFileSystem()->resolve("./scene.clvscene"));
    }

    void EditorSubSystem::loadScene() {
        currentScene.load(clove::Application::get().getFileSystem()->resolve("./scene.clvscene"));

        Engine_OnSceneLoaded ^ loadMessage { gcnew Engine_OnSceneLoaded };
        loadMessage->entities = gcnew System::Collections::Generic::List<Entity ^>{};
        for(auto entity : currentScene.getKnownEntities()) {
            Entity ^ editorEntity { gcnew Entity };
            editorEntity->id         = entity;
            editorEntity->name       = gcnew System::String(currentScene.getComponent<NameComponent>(entity).name.c_str());
            editorEntity->components = gcnew System::Collections::Generic::List<Component ^>{};

            //Add all of the component types for an entity
            if(currentScene.hasComponent<clove::TransformComponent>(entity)) {
                auto const &transform{ currentScene.getComponent<clove::TransformComponent>(entity) };

                auto const &pos{ transform.position };
                auto const &rot{ clove::quaternionToEuler(transform.rotation) };
                auto const &scale{ transform.scale };

                TransformComponentInitData ^ initData { gcnew TransformComponentInitData{} };
                initData->position = Vector3(pos.x, pos.y, pos.z);
                initData->rotation = Vector3(rot.x, rot.y, rot.z);
                initData->scale    = Vector3(scale.x, scale.y, scale.z);

                Component ^ componentData { gcnew Component{} };
                componentData->type = ComponentType::Transform;
                componentData->initData = initData;

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

    clove::Entity EditorSubSystem::createEntity(std::string_view name) {
        clove::Entity entity{ currentScene.createEntity() };
        currentScene.addComponent<NameComponent>(entity, std::string{ std::move(name) });

        Engine_OnEntityCreated ^ message { gcnew Engine_OnEntityCreated };
        message->entity = entity;
        message->name   = gcnew System::String(std::string{ std::begin(name), std::end(name) }.c_str());
        MessageHandler::sendMessage(message);

        return entity;
    }

    void EditorSubSystem::deleteEntity(clove::Entity entity) {
        currentScene.deleteEntity(entity);

        Engine_OnEntityDeleted ^ message { gcnew Engine_OnEntityDeleted };
        message->entity = entity;
        MessageHandler::sendMessage(message);
    }

    void EditorSubSystem::addComponent(clove::Entity entity, ComponentType componentType) {
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
                    currentScene.addComponent<clove::StaticModelComponent>(entity);
                    added    = true;
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
            Engine_OnComponentAdded ^ message { gcnew Engine_OnComponentAdded };
            message->entity        = entity;
            message->componentType = componentType;
            message->data          = initData;
            MessageHandler::sendMessage(message);
        }
    }

    void EditorSubSystem::removeComponent(clove::Entity entity, ComponentType componentType) {
        switch(componentType) {
            case ComponentType::Transform:
                currentScene.removeComponent<clove::TransformComponent>(entity);
                break;
            case ComponentType::StaticModel:
                currentScene.removeComponent<clove::StaticModelComponent>(entity);
                break;
            case ComponentType::PointLight:
                currentScene.removeComponent<clove::PointLightComponent>(entity);
                break;
            case ComponentType::RigidBody:
                currentScene.removeComponent<clove::RigidBodyComponent>(entity);
                break;
            case ComponentType::CollisionShape:
                currentScene.removeComponent<clove::CollisionShapeComponent>(entity);
                break;
        }

        Engine_OnComponentRemoved ^ message { gcnew Engine_OnComponentRemoved };
        message->entity        = entity;
        message->componentType = componentType;
        MessageHandler::sendMessage(message);
    }

    void EditorSubSystem::updateTransform(clove::Entity entity, clove::vec3f position, clove::vec3f rotation, clove::vec3f scale) {
        if(currentScene.hasComponent<clove::TransformComponent>(entity)) {
            auto &transform{ currentScene.getComponent<clove::TransformComponent>(entity) };
            transform.position = position;
            transform.rotation = rotation;
            transform.scale    = scale;
        }
    }

    void EditorSubSystem::updateStaticModel(clove::Entity entity, std::string meshPath, std::string diffusePath, std::string specularPath) {
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

    void EditorSubSystem::updateRigidBody(clove::Entity entity, float mass) {
        if(currentScene.hasComponent<clove::RigidBodyComponent>(entity)) {
            auto &rigidBody{ currentScene.getComponent<clove::RigidBodyComponent>(entity) };
            rigidBody.mass = mass;
        }
    }

    void EditorSubSystem::updateSphereShape(clove::Entity entity, float radius) {
        if(currentScene.hasComponent<clove::CollisionShapeComponent>(entity)) {
            currentScene.getComponent<clove::CollisionShapeComponent>(entity).shape = clove::CollisionShapeComponent::Sphere{ radius };
        }
    }

    void EditorSubSystem::updateCubeShape(clove::Entity entity, clove::vec3f halfExtents) {
        if(currentScene.hasComponent<clove::CollisionShapeComponent>(entity)) {
            currentScene.getComponent<clove::CollisionShapeComponent>(entity).shape = clove::CollisionShapeComponent::Cube{ halfExtents };
        }
    }

    void EditorSubSystem::updateName(clove::Entity entity, std::string name) {
        if(currentScene.hasComponent<NameComponent>(entity)) {
            currentScene.getComponent<NameComponent>(entity).name = std::move(name);
        }
    }
}