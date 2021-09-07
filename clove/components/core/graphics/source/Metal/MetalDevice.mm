#include "Clove/Graphics/Metal/MetalDevice.hpp"

#include "Clove/Graphics/Metal/MetalFactory.hpp"
#include "Clove/Graphics/Metal/MetalView.hpp"
#include "Clove/Graphics/Metal/DeviceWrapper.hpp"

#include <MetalKit/MetalKit.h>

namespace clove {
    MetalDevice::MetalDevice(std::unique_ptr<DeviceWrapper> deviceWrapper, std::unique_ptr<MetalFactory> factory) 
        : wrapper{ std::move(deviceWrapper) }
        , factory{ std::move(factory) }{
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
    
    GhaDevice::Info MetalDevice::getInfo() const {
        return Info {
            .ApiName = "Metal",
            .deviceName = [wrapper->device.name UTF8String],
        };
    }
    
    GhaDevice::Limits MetalDevice::getLimits() const {
        size_t constexpr metalMinUboOffsetAlignment{ 16 };//Hard coding as a minimum of 16 as metal does not provide this data.
        
        return Limits {
            .minUniformBufferOffsetAlignment = metalMinUboOffsetAlignment,
        };
    }
}
