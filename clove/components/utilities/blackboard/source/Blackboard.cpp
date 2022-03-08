#include "Clove/Blackboard.hpp"

namespace {
    size_t constexpr defaultMemorySize{ 1024 };
}

namespace clove {
    Blackboard::Blackboard()
        : Blackboard{ defaultMemorySize } {
    }

    Blackboard::Blackboard(size_t memorySize)
        : memoryBlock{ memorySize } {
    }

    Blackboard::Blackboard(Blackboard const &other)
        : memoryBlock{ other.memoryBlock.getSize() } {
        std::byte *const memoryStart{ const_cast<std::byte *>(memoryBlock.getMemory()) };//NOLINT Horrible const_cast required here as ListAllocator::getMemory should return const
        std::byte const *const otherMemoryStart{ other.memoryBlock.getMemory() };

        memcpy(memoryStart, otherMemoryStart, other.memoryBlock.getSize());

        for(auto &&[key, ptr] : other.dataMap){
            intptr_t const ptrDiff{ reinterpret_cast<std::byte *>(ptr) - otherMemoryStart };

            dataMap[key] = memoryStart + ptrDiff;
        }
    }

    Blackboard::Blackboard(Blackboard &&other) noexcept
        : memoryBlock{ std::move(other.memoryBlock) }
        , dataMap{ std::move(other.dataMap) } {
    }

    Blackboard &Blackboard::operator=(Blackboard const &other) {
        memoryBlock = { other.memoryBlock.getSize() };

        std::byte *const memoryStart{ const_cast<std::byte *>(memoryBlock.getMemory()) };//NOLINT Horrible const_cast required here as ListAllocator::getMemory should return const
        std::byte const *const otherMemoryStart{ other.memoryBlock.getMemory() };

        memcpy(memoryStart, otherMemoryStart, other.memoryBlock.getSize());

        for(auto &&[key, ptr] : other.dataMap) {
            intptr_t const ptrDiff{ reinterpret_cast<std::byte *>(ptr) - otherMemoryStart };

            dataMap[key] = memoryStart + ptrDiff;
        }

        return *this;
    }

    Blackboard &Blackboard::operator=(Blackboard &&other) noexcept {
        memoryBlock = std::move(other.memoryBlock);
        dataMap     = std::move(other.dataMap);

        return *this;
    }

    Blackboard::~Blackboard() = default;

    bool Blackboard::hasValue(Key key) const {
        return dataMap.find(key) != dataMap.end();
    }
}