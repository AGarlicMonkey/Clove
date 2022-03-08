#pragma once

#include <Clove/Blackboard.hpp>
#include <optional>
#include <string>
#include <tuple>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace clove::reflection {
    class AttributeContainer {
        //VARIABLES
    private:
        Blackboard attributes{};

        //FUNCTIONS
    public:
        AttributeContainer();

        AttributeContainer(AttributeContainer const &other);
        AttributeContainer(AttributeContainer &&other) noexcept;

        AttributeContainer &operator=(AttributeContainer const &other);
        AttributeContainer &operator=(AttributeContainer &&other) noexcept;

        ~AttributeContainer();

        template<typename AttributeType>
        void add(AttributeType &&attribute);

        template<typename AttributeType>
        bool contains() const;

        template<typename AttributeType>
        std::optional<AttributeType> get() const;
    };

    using TypeId = decltype(std::declval<std::type_info>().hash_code());

    /**
     * @brief A member within a TypeInfo.
     */
    struct MemberInfo {
        std::string name{};
        TypeId id{};     /**< Type id for this member. If the member is reflected then this ID can be used to retrieve the TypeInfo struct. */
        size_t offset{}; /**< Offset of this member within the type it is contained in. */
        size_t size{};   /**< Total size of this member. */
        AttributeContainer attributes{};
    };

    /**
     * @brief Info about a reflected type.
     */
    struct TypeInfo {
        std::string name{};
        TypeId id{}; /**< Type id for this type info. Automatically generated using typeid */
        size_t size{};
        std::vector<MemberInfo> members;
        AttributeContainer attributes{};
    };

    /**
     * @brief Returns the TypeInfo for the provided Type
     * @tparam Type 
     * @return 
     */
    template<typename Type>
    TypeInfo const *getTypeInfo();

    /**
     * @brief Returns the TypeInfo for the provided typeName.
     * @param typeName 
     * @return 
     */
    TypeInfo const *getTypeInfo(std::string_view typeName);

    /**
     * @brief Returns the TypeInfo for the provided typeId.
     * @param typeId 
     * @return 
     */
    TypeInfo const *getTypeInfo(TypeId typeId);

    /**
     * @brief Returns all types with attribute of AttributeType
     * @tparam AttributeType 
     * @return 
     */
    template<typename AttributeType>
    std::vector<TypeInfo const *> getTypesWithAttribute();

    namespace internal {
        class Registry {
            //VARIABLES
        private:
            std::unordered_map<TypeId, reflection::TypeInfo> types{};

            //FUNCTIONS
        public:
            Registry();

            Registry(Registry const &other)     = delete;
            Registry(Registry &&other) noexcept = delete;

            Registry &operator=(Registry const &other) = delete;
            Registry &operator=(Registry &&other) noexcept = delete;

            ~Registry();

            static Registry &get();

            inline std::unordered_map<TypeId, reflection::TypeInfo> const &getRegisteredTypes() const;

            void addTypeInfo(TypeId const typeId, reflection::TypeInfo typeInfo);
        };

        template<typename ReflectType>
        struct CreatorHelper;

        template<size_t index, typename TupleType>
        static void populateAttributes(AttributeContainer &container, TupleType const &attributeTuple);
    }
}

//Custom sizeof / typeid. GCC does not allow sizeof(Type::member)
#define INTERAL_CLOVE_MEMBER_SIZE(type, member) sizeof(((type *)0)->member)
#define INTERAL_CLOVE_MEMBER_TYPEID(type, member) typeid(((type *)0)->member)

#define INTERNAL_CLOVE_REFLECT_CAT2(a, b) a##b
#define INTERNAL_CLOVE_REFLECT_CAT(a, b) INTERNAL_CLOVE_REFLECT_CAT2(a, b)

/**
 * @brief Placed in the header of a class signifying that it will be reflected.
 * @details This should always be called before CLOVE_REFLECT_BEGIN etc.
 */
#define CLOVE_REFLECT_DECLARE_TYPE(classType)                      \
    template<>                                                     \
    struct clove::reflection::internal::CreatorHelper<classType> { \
        CreatorHelper();                                           \
    };                                                             \
    static inline clove::reflection::internal::CreatorHelper<classType> const INTERNAL_CLOVE_REFLECT_CAT(creator, __COUNTER__){};

#define CLOVE_REFLECT_BEGIN(classType, ...)                                  \
    clove::reflection::internal::CreatorHelper<classType>::CreatorHelper() { \
        using Type = classType;                                              \
        ::clove::reflection::TypeInfo info{};                                \
        info.name = #classType;                                              \
        info.id   = typeid(classType).hash_code();                           \
        info.size = sizeof(classType);                                       \
        ::clove::reflection::internal::populateAttributes<0>(info.attributes, std::make_tuple(__VA_ARGS__));

#define CLOVE_REFLECT_MEMBER(member, ...)                                                                          \
    {                                                                                                              \
        ::clove::reflection::MemberInfo memberInfo{};                                                              \
        memberInfo.name   = #member;                                                                               \
        memberInfo.id     = INTERAL_CLOVE_MEMBER_TYPEID(Type, member).hash_code();                                 \
        memberInfo.offset = offsetof(Type, member);                                                                \
        memberInfo.size   = INTERAL_CLOVE_MEMBER_SIZE(Type, member);                                               \
        ::clove::reflection::internal::populateAttributes<0>(memberInfo.attributes, std::make_tuple(__VA_ARGS__)); \
        info.members.push_back(std::move(memberInfo));                                                             \
    }

#define CLOVE_REFLECT_END                                                                                \
    clove::reflection::internal::Registry::get().addTypeInfo(typeid(Type).hash_code(), std::move(info)); \
    }                                                                                                    \
    ;

/**
 * @brief Allows reflection of private members within a class.
 */
#define CLOVE_REFLECT_PRIVATE(classType) \
    friend struct ::clove::reflection::internal::CreatorHelper<classType>;

#include "Reflection.inl"