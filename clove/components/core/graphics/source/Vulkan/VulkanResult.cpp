#include "Clove/Graphics/Vulkan/VulkanResult.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

namespace clove {
    Result convertResult(VkResult result) {
        switch(result) {
            case VK_SUCCESS:
                return Result::Success;
            case VK_SUBOPTIMAL_KHR:
                return Result::Success_SwapchainSuboptimal;
            case VK_ERROR_OUT_OF_DATE_KHR:
                return Result::Error_SwapchainOutOfDate;
            default:
                CLOVE_ASSERT(false, "Unkown result type");
                return Result::Unkown;
        }
    }
}