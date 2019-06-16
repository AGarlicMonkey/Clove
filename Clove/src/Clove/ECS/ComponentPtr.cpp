#include "clvpch.hpp"
#include "ComponentPtr.hpp"

#include "Clove/ECS/Manager.hpp"

namespace clv::ecs{
	ComponentPtrBase::ComponentPtrBase() = default;

	ComponentPtrBase::ComponentPtrBase(const ComponentPtrBase& other) = default;

	ComponentPtrBase::ComponentPtrBase(ComponentPtrBase&& other) noexcept = default;

	ComponentPtrBase& ComponentPtrBase::operator=(const ComponentPtrBase& other) = default;

	ComponentPtrBase& ComponentPtrBase::operator=(ComponentPtrBase&& other) noexcept = default;

	ComponentPtrBase::~ComponentPtrBase() = default;

	ComponentPtrBase::ComponentPtrBase(Manager* manager, EntityID entityID)
		: manager(manager)
		, entityID(entityID){
	}

	bool ComponentPtrBase::isValid(ComponentID ID) const{
		if(entityID == INVALID_ENTITY_ID){
			return false;
		}

		const auto it = manager->entities.find(entityID);
		if(it == manager->entities.end()){
			return false;
		}

		return manager->entities[entityID]->components.find(ID) != manager->entities[entityID]->components.end();
	}

	Component* ComponentPtrBase::getComponent(ComponentID ID){
		return manager->entities[entityID]->components[ID].get();
	}
}