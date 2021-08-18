#pragma once

#include <Clove/Memory/ListAllocator.hpp>
#include <map>
#include <optional>

namespace clove {
    class BlackBoard {
        //TYPES
    public:
        using Key = size_t;

        //VARIABLES
    private:
        ListAllocator memoryBlock;

        std::map<Key, void *> dataMap;

        //FUNCTIONS
    public:
        BlackBoard();
        BlackBoard(size_t memorySize);

        BlackBoard(BlackBoard const &other) = delete;
        BlackBoard(BlackBoard &&other) noexcept;

        BlackBoard &operator=(BlackBoard const &other) = delete;
        BlackBoard &operator=(BlackBoard &&other) noexcept;

        ~BlackBoard();

        template<typename DataType>
        void setValue(Key key, DataType value);
        template<typename DataType>
        std::optional<DataType> getValue(Key key);
    };
}

#include "BlackBoard.inl"