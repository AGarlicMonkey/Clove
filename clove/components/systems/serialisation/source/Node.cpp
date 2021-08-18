#include "Clove/Serialisation/Node.hpp"

namespace clove::serialiser {
    Node::Node() = default;

    Node::Node(Node const &other) = default;

    Node::Node(Node &&other) noexcept = default;

    Node &Node::operator=(Node const &other) {
        if(other.type == Type::Leaf) {
            scalar = other.scalar;
        }
        nodes = other.nodes;
        type  = other.type;

        return *this;
    }

    Node &Node::operator=(Node &&other) noexcept {
        if(other.type == Type::Leaf) {
            scalar = std::move(other.scalar);
        }
        nodes = std::move(other.nodes);
        type  = other.type;

        return *this;
    }

    Node &Node::operator=(char const *string) {
        *this = operator=<char const *>(string);
        return *this;
    }

    Node::~Node() = default;

    Node &Node::operator[](std::string_view nodeName) {
        if(type == Type::None) {
            type = Type::Map;
            nodes.clear();
        } else if(type == Type::Sequence) {
            type = Type::Map;
            for(size_t i{ 0 }; i < nodes.size(); ++i) {
                nodes[i].scalar = std::to_string(i);
            }
        }

        if(type == Type::Map) {
            for(auto &node : nodes) {
                if(node.scalar == nodeName) {
                    return node;
                }
            }

            return nodes.emplace_back(Node{ nodeName });
        } else {
            throw std::runtime_error{ "Cannot pushBack onto a non-sequence type node." };
        }
    }

    Node const &Node::operator[](std::string_view nodeName) const {
        if(type != Type::Map) {
            throw std::runtime_error{ "Node is const and is not of type Map. Cannot perform any valid conversion" };
        }

        for(auto const &node : nodes) {
            if(node.scalar == nodeName) {
                return node;
            }
        }

        throw std::runtime_error{ "Node does not have requested child." };
    }

    Node::Node(std::string_view key)
        : scalar{ key } {
    }
}