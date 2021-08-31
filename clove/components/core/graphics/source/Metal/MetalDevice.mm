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
        if(nativeWindow.has_value()) {
            NSWindow *nsWindow{ std::any_cast<NSWindow *>(nativeWindow) };
            wrapper->view = [[MetalView alloc] initWithFrame:[nsWindow frame]];
            [wrapper->view setDevice:wrapper->device];
        
            [nsWindow setContentView:wrapper->view];
        }else {
            wrapper->view = nullptr;
        }
        
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
        //TODO
        return {};
    }
}
