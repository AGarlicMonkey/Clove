#pragma once

#include <Clove/Log/Log.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace clove::serialiser {
    /**
     * @brief Each serialisation node represents a value deserialised from a file.
     */
    class Node {
        //TYPES
    public:
        using VectorType = std::vector<Node>;

        /**
         * @brief Dictates what the child / children of this node are.
         */
        enum class Type {
            None,     /**< Node is empty and has not been assigned anything. */
            Leaf,     /**< Node is a leaf node. It's scalar member will be a value. */
            Scalar,   /**< Node is a value. Has a single node of Type::Leaf that contains the value. */
            Sequence, /**< Node is a series of values. Has multiple nodes of Type::Leaf that contain values. */
            Map,      /**< Node is a series of key - value pairs. Has multiple nodes of Type::Scalar that contain values. */
        };

        //VARIABLES
    private:
        std::string scalar{};
        VectorType nodes{};

        Type type{ Type::None };

        //FUNCTIONS
    public:
        Node();

        Node(Node const &other);
        Node(Node &&other) noexcept;

        Node &operator=(Node const &other);
        Node &operator=(Node &&other) noexcept;

        template<typename T>
        Node &operator=(T const &value);
        Node &operator=(char const *string);

        ~Node();

        /**
         * @brief Pushes a value onto this node. Turning it into a sequence.
         */
        template<typename T>
        void pushBack(T const &scalar);

        inline Type getType() const;
        inline std::string getKey() const;
        inline size_t numChildren() const;

        inline VectorType::iterator begin();
        inline VectorType::iterator end();

        inline VectorType::const_iterator begin() const;
        inline VectorType::const_iterator end() const;

        /**
         * @brief Converts this node into T.
         */
        template<typename T>
        T as() const;

        inline Node &operator[](size_t index);
        inline Node const &operator[](size_t index) const;

        Node &operator[](std::string_view nodeName);
        Node const &operator[](std::string_view nodeName) const;

    private:
        Node(std::string_view key);

        /**
         * @brief Sets the value of this node. Turning it into a leaf node.
         */
        template<typename T>
        void setValue(T const &value);
    };
}

namespace clove {
    template<typename UnkownType>
    serialiser::Node serialise(UnkownType const &object) {
        CLOVE_ASSERT(false, "No template specialisation provided");
        return {};
    }

    template<typename UnkownType>
    UnkownType deserialise(serialiser::Node const &node) {
        CLOVE_ASSERT(false, "No template specialisation provided");
        return {};
    }
}

#include "Node.inl"