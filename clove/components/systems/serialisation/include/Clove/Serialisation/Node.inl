#include <sstream>
#include <stdexcept>

namespace clove::serialiser {
    namespace detail {
        template<typename T>
        inline bool constexpr isKeyType = std::is_same_v<std::string, T> || std::is_same_v<std::string_view, T>;

        template<>
        inline bool constexpr isKeyType<char const *> = true;

        template<size_t N>
        inline bool constexpr isKeyType<char const[N]> = true;
    }
    
    template<typename T>
    Node &Node::operator=(T const &value) {
        if constexpr(std::is_arithmetic_v<T> || detail::isKeyType<T>) {
            type = Type::Scalar;
            nodes.resize(1);
            nodes[0].setValue(value);
        } else {
            *this = serialise(value);
        }

        return *this;
    }

    template<typename T>
    void Node::pushBack(T const &scalar) {
        if(type == Type::None){
            type = Type::Sequence;
            nodes.clear();
        }

        if(type == Type::Sequence) {
            Node node{};
            node = scalar;
            nodes.push_back(std::move(node));
        } else if(type == Type::Map) {
            size_t const index{ nodes.size() };
            (*this)[std::to_string(index)] = scalar;
        } else {
            throw std::runtime_error{ "Cannot pushBack onto a non-sequence type node." };
        }
    }

    Node::Type Node::getType() const {
        return type;
    }

    std::string Node::getKey() const {
        return scalar;
    }

    size_t Node::numChildren() const {
        return nodes.size();
    }

    Node::VectorType::iterator Node::begin() {
        return nodes.begin();
    }

    Node::VectorType::iterator Node::end() {
        return nodes.end();
    }

    Node::VectorType::const_iterator Node::begin() const {
        return nodes.begin();
    }

    Node::VectorType::const_iterator Node::end() const {
        return nodes.end();
    }

    Node &Node::operator[](size_t index) {
        return nodes[index];
    }

    Node const &Node::operator[](size_t index) const {
        return nodes[index];
    }

    template<typename T>
    T Node::as() const {
        if constexpr(std::is_arithmetic_v<T>) {
            if(type != Type::Scalar) {
                throw std::runtime_error{ "Node is not holding a value!" };
            }
            std::stringstream stream{};
            stream.precision(std::numeric_limits<T>::max_digits10);
            stream.str(nodes[0].scalar);

            T value{};
            stream >> value;

            return value;
        } else if constexpr(std::is_same_v<std::string, T>) {
            if(type != Type::Scalar) {
                throw std::runtime_error{ "Node is not holding a value!" };
            }
            return nodes[0].scalar;
        } else {
            if(type != Type::Sequence && type != Type::Map) {
                throw std::runtime_error{ "Node is not holding any nodes!" };
            }
            return deserialise<T>(*this);
        }
    }

    template<typename T>
    void Node::setValue(T const &value) {
        type = Type::Leaf;
        nodes.clear();

        if constexpr(std::is_arithmetic_v<T>) {
            std::stringstream stream{};
            stream.precision(std::numeric_limits<T>::max_digits10);
            stream << value;
            scalar = stream.str();
        } else if constexpr(detail::isKeyType<T>) {
            scalar = value;
        } else {
            throw std::runtime_error{ "setValue called without a scalar value." };
        }
    }
}
