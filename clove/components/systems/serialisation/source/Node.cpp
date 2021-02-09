#include "Clove/Serialisation/Node.hpp"

namespace garlic::clove::serialiser {
    Node::Node() = default;

    Node::Node(Node const &other) = default;

    Node::Node(Node &&other) noexcept = default;

    Node &Node::operator=(Node const &other) = default;

    Node &Node::operator=(Node &&other) noexcept = default;

    Node::~Node() = default;

    Node &Node::operator[](std::string_view nodeName) {
        //TODO: Seq -> Map conversion
        type = Type::Map;

        for(auto &node : nodes) {
            if(node.scalar == nodeName) {
                return node.nodes[0];
            }
        }

        return nodes.emplace_back(Node{ nodeName }).nodes[0];
    }

    Node const &Node::operator[](std::string_view nodeName) const {
        if(type != Type::Scalar || type != Type::Map) {
            throw std::runtime_error{ "Node does not contain child nodes." };
        }

        for(auto const &node : nodes) {
            if(node.scalar == nodeName) {
                return node.nodes[0];
            }
        }

        throw std::runtime_error{ "Node does not have requested child." };
    }

    Node::Node(std::string_view key)
        : scalar{ key } {
        Node child{};
        child.type = Type::Scalar;
        nodes.emplace_back(std::move(child));
    }
}