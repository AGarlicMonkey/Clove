#include "Clove/Graphics/Metal/MetalGha.hpp"

#include "Clove/Graphics/Metal/DeviceWrapper.hpp"
#include "Clove/Graphics/Metal/MetalFactory.hpp"
#include "Clove/Graphics/Metal/MetalDevice.hpp"

#include <MetalKit/MetalKit.h>

namespace clove {
    Expected<std::unique_ptr<MetalDevice>, std::runtime_error> createMetalDevice(std::any nativeWindow) noexcept {
        auto wrapper{ std::make_unique<DeviceWrapper>() };
        
        //Create device
        wrapper->device = MTLCreateSystemDefaultDevice();
        
        //Create view
        if(nativeWindow.has_value()) {
            NSWindow *nsWindow{ std::any_cast<NSWindow *>(nativeWindow) };
            wrapper->view = [[MetalView alloc] initWithFrame:[nsWindow frame]];
            [wrapper->view setDevice:wrapper->device];
        
            [nsWindow setContentView:wrapper->view];
        }else {
            wrapper->view = nullptr;
        }
        
        auto factory{ std::make_unique<MetalFactory>(wrapper->device, wrapper->view) };

        return std::make_unique<MetalDevice>(std::move(wrapper), std::move(factory));
    }
}
