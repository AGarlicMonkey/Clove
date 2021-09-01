#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

CLOVE_DECLARE_LOG_CATEGORY(CloveAi)

namespace clove {
    template<typename DataType>
    void BlackBoard::setValue(Key key, DataType value) {
        if(dataMap.find(key) == dataMap.end()) {
            void *block = memoryBlock.alloc<DataType>();
            if(block == nullptr) {
                CLOVE_LOG(CloveAi, LogLevel::Error, "{0}: Could not allocate space for new item", CLOVE_FUNCTION_NAME);
                return;
            }

            dataMap[key] = block;
        }

        *reinterpret_cast<DataType *>(dataMap[key]) = value;
    }

    template<typename DataType>
    std::optional<DataType> BlackBoard::getValue(Key key) {
        if(dataMap.find(key) == dataMap.end()) {
            return {};
        }

        return { *reinterpret_cast<DataType *>(dataMap[key]) };
    }
}