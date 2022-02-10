#include "Clove/Reflection/Reflection.hpp"

#include <Clove/Log/Log.hpp>

CLOVE_DECLARE_LOG_CATEGORY(CloveReflection)

static clove::reflection::internal::Registry *instance{ nullptr };

#if CLOVE_PLATFORM_WINDOWS
extern "C" {
__declspec(dllexport) void setUpReflector(clove::reflection::internal::Registry *reg) {
    std::unordered_map<clove::reflection::TypeId, clove::reflection::TypeInfo> types{ clove::reflection::internal::Registry::get().getRegisteredTypes() };

    for(auto &&[id, info] : types) {
        reg->addTypeInfo(id, info);
    }

    //delete instance;
    instance = reg;
}
}
#endif

namespace clove::reflection {
    AttributeContainer::AttributeContainer() = default;

    AttributeContainer::AttributeContainer(AttributeContainer const &other) = default;

    AttributeContainer::AttributeContainer(AttributeContainer &&other) noexcept = default;

    AttributeContainer &AttributeContainer::operator=(AttributeContainer const &other) = default;

    AttributeContainer &AttributeContainer::operator=(AttributeContainer &&other) noexcept = default;

    AttributeContainer::~AttributeContainer() = default;

    TypeInfo const *getTypeInfo(std::string_view typeName) {
        for(auto &&[typeId, typeInfo] : internal::Registry::get().getRegisteredTypes()) {
            if(typeInfo.name == typeName) {
                return &typeInfo;
            }
        }

        return nullptr;
    }

    TypeInfo const *getTypeInfo(TypeId typeId) {
        auto const &registeredTypes{ internal::Registry::get().getRegisteredTypes() };

        if(registeredTypes.contains(typeId)) {
            return &registeredTypes.at(typeId);
        } else {
            return nullptr;
        }
    }

    namespace internal {
        Registry::Registry() = default;

        Registry::~Registry() = default;

        Registry &Registry::get() {
            if(instance == nullptr) {
                instance = new Registry{};
            }

            return *instance;
        }

        void Registry::addTypeInfo(TypeId const typeId, reflection::TypeInfo typeInfo) {
            if(!types.contains(typeId)) {
                types.emplace(typeId, std::move(typeInfo));
            }
        }
    }
}