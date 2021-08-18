#include "Clove/AI/BlackBoard.hpp"

namespace {
    size_t constexpr defaultMemorySize{ 1024 };
}

namespace clove {
    BlackBoard::BlackBoard()
        : BlackBoard(defaultMemorySize) {
    }

    BlackBoard::BlackBoard(size_t memorySize)
        : memoryBlock(memorySize) {
    }

    BlackBoard::BlackBoard(BlackBoard &&other) noexcept
        : memoryBlock(std::move(other.memoryBlock)) {
        dataMap = std::move(other.dataMap);
    }

    BlackBoard &BlackBoard::operator=(BlackBoard &&other) noexcept {
        memoryBlock = std::move(other.memoryBlock);
        dataMap     = std::move(other.dataMap);

        return *this;
    }

    BlackBoard::~BlackBoard() = default;
}