#pragma once

#include <Clove/Memory/ListAllocator.hpp>
#include <map>
#include <optional>

namespace clove {
    class Blackboard {
        //TYPES
    public:
        using Key = size_t;

        //VARIABLES
    private:
        ListAllocator memoryBlock;
        std::map<Key, void *> dataMap{};

        //FUNCTIONS
    public:
        Blackboard();
        Blackboard(size_t memorySize);

        Blackboard(Blackboard const &other);
        Blackboard(Blackboard &&other) noexcept;

        Blackboard &operator=(Blackboard const &other);
        Blackboard &operator=(Blackboard &&other) noexcept;

        ~Blackboard();

        template<typename DataType>
        void setValue(Key key, DataType value);
        bool hasValue(Key key) const;
        template<typename DataType>
        std::optional<DataType> getValue(Key key) const;
    };
}

#include "Blackboard.inl"