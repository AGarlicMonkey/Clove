#include <type_traits>

namespace clove::reflection {
    template<typename AttributeType>
    void AttributeContainer::add(AttributeType &&attribute) {
        attributes.setValue(typeid(AttributeType).hash_code(), std::forward<AttributeType>(attribute));
    }

    template<typename AttributeType>
    bool AttributeContainer::contains() const {
        return attributes.hasValue(typeid(AttributeType).hash_code());
    }

    template<typename AttributeType>
    std::optional<AttributeType> AttributeContainer::get() const {
        return attributes.getValue<AttributeType>(typeid(AttributeType).hash_code());
    }

    template<typename Type>
    TypeInfo const *getTypeInfo() {
        return getTypeInfo(typeid(Type).hash_code());
    }

    template<typename AttributeType>
    std::vector<TypeInfo const *> getTypesWithAttribute() {
        std::vector<TypeInfo const *> typeInfos{};

        for(auto &&[typeId, typeInfo] : internal::Registry::get().getRegisteredTypes()) {
            if(typeInfo.attributes.template contains<AttributeType>()) {//template required here as GCC gets confused with the triangle brakets
                typeInfos.push_back(&typeInfo);
            }
        }

        return typeInfos;
    }

    namespace internal {
        std::unordered_map<TypeId, reflection::TypeInfo> const &Registry::getRegisteredTypes() const {
            return types;
        }

        template<size_t index, typename TupleType>
        static void populateAttributes(AttributeContainer &container, TupleType const &attributeTuple) {
            if constexpr(index < std::tuple_size_v<TupleType>) {
                container.add(std::get<index>(attributeTuple));

                populateAttributes<index + 1>(container, attributeTuple);
            }
        }
    }
}