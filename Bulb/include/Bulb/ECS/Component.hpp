#pragma once

#include "Bulb/ECS/ECSTypes.hpp"

#include <list>

namespace blb::ecs {
	template<typename ComponentType>
	class ComponentPtr;

	template<typename ComponentType>
	class ComponentContainer;
}

namespace blb::ecs {
	class ComponentInterface {
		//FUNCTIONS
	public:
		virtual ~ComponentInterface() = default;

		virtual ComponentID getComponentID() const = 0;
	};

	template<typename DerivedClassType>
	class Component : public ComponentInterface {
		friend class ComponentPtr<DerivedClassType>;
		friend class ComponentContainer<DerivedClassType>;

		//VARIABLES
	private:
		EntityID entityID = INVALID_ENTITY_ID;

		std::list<ComponentPtr<DerivedClassType>*> pointers;

		//FUNCTIONS
	public:
		Component();

		Component(const Component& other);
		Component(Component&& other) noexcept;

		Component& operator=(const Component& other);
		Component& operator=(Component&& other) noexcept;

		virtual ~Component();

		static ComponentID id();
		virtual ComponentID getComponentID() const override;

		EntityID getEntityID() const;

	private:
		void attachPointer(ComponentPtr<DerivedClassType>* ptr);
		void detachPointer(ComponentPtr<DerivedClassType>* ptr);
	};

	template<typename ComponentType>
	class ComponentPtr {
		friend class Component<ComponentType>;

		//VARIABLES
	private:
		//Having this base pointer allows us to use incomplete ComponentTypes
		Component<ComponentType>* base = nullptr;
		ComponentType* component = nullptr;

		//FUNCTIONS
	public:
		ComponentPtr();
		ComponentPtr(ComponentType* component);

		ComponentPtr(const ComponentPtr& other);
		ComponentPtr(ComponentPtr&& other) noexcept;

		ComponentPtr& operator=(const ComponentPtr& other);
		ComponentPtr& operator=(ComponentPtr&& other) noexcept;

		~ComponentPtr();

		bool isValid() const;

		void reset();

		ComponentType* get() const;

		ComponentType* operator->() const;
		ComponentType& operator*() const;

		operator bool() const;

	private:
		void copy(const ComponentPtr& other);
		void move(ComponentPtr&& other);
	};
}

#include "Component.inl"

//ComponentType == ComponentType
template<typename ComponentType>
inline bool operator==(const blb::ecs::ComponentPtr<ComponentType>& lptr, const blb::ecs::ComponentPtr<ComponentType>& rptr) {
	return lptr.get() == rptr.get();
}
template<typename ComponentType>
inline bool operator==(const blb::ecs::ComponentPtr<ComponentType>& ptr, const ComponentType* object) {
	return ptr.get() == object;
}
template<typename ComponentType>
inline bool operator==(const blb::ecs::ComponentPtr<ComponentType>& ptr, const ComponentType& object) {
	return ptr.get() == *object;
}

//ComponentType != ComponentType
template<typename ComponentType>
inline bool operator!=(const blb::ecs::ComponentPtr<ComponentType>& lptr, const blb::ecs::ComponentPtr<ComponentType>& rptr) {
	return !(lptr == rptr);
}
template<typename ComponentType>
inline bool operator!=(const blb::ecs::ComponentPtr<ComponentType>& ptr, const ComponentType* object) {
	return !(ptr == object);
}
template<typename ComponentType>
inline bool operator!=(const blb::ecs::ComponentPtr<ComponentType>& ptr, const ComponentType& object) {
	return !(ptr == object);
}

//ComponentType == nullptr_t
template<typename ComponentType>
inline bool operator==(const blb::ecs::ComponentPtr<ComponentType>& ptr, std::nullptr_t object) {
	return ptr.get() == object;
}

//ComponentType != nullptr_t
template<typename ComponentType>
inline bool operator!=(const blb::ecs::ComponentPtr<ComponentType>& ptr, std::nullptr_t object) {
	return !(ptr == object);
}