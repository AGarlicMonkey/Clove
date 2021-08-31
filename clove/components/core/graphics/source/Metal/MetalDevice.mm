#include "Clove/Graphics/Metal/MetalDevice.hpp"

#include "Clove/Graphics/Metal/MetalFactory.hpp"
#include "Clove/Graphics/Metal/MetalView.hpp"

namespace clove {
    struct DeviceWrapper {
        id<MTLDevice> device{ nullptr };
        MetalView *view{ nullptr };
    };
}

namespace clove {
    MetalDevice::MetalDevice(std::any nativeWindow) {
        wrapper = std::make_unique<DeviceWrapper>();
        
        //Create device
        wrapper->device = MTLCreateSystemDefaultDevice();
        
        //Create view
        NSWindow *nsWindow{ std::any_cast<NSWindow *>(nativeWindow) };
        wrapper->view = [[MetalView alloc] initWithFrame:[nsWindow frame]];
        [wrapper->view setDevice:wrapper->device];
        
        [nsWindow setContentView:wrapper->view];
        
        factory = std::make_unique<MetalFactory>(wrapper->device, wrapper->view);
    }
    
    MetalDevice::MetalDevice(MetalDevice &&other) noexcept = default;
    
    MetalDevice &MetalDevice::operator=(MetalDevice &&other) noexcept = default;
    
    MetalDevice::~MetalDevice() = default;
    
    GhaFactory *MetalDevice::getGraphicsFactory() const {
        return factory.get();
    }
    
    void MetalDevice::waitForIdleDevice() {
        //No op
    }
    
    GhaDevice::Limits MetalDevice::getLimits() const {
        size_t constexpr metalMinUboOffsetAlignment{ 16 };//Hard coding as a minimum of 16 as metal does not provide this data.
        
        return Limits {
            .minUniformBufferOffsetAlignment = metalMinUboOffsetAlignment,
        };
    }
}
