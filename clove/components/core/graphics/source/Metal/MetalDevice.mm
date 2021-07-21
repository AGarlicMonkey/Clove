#include "Clove/Graphics/Metal/MetalDevice.hpp"

#include "Clove/Graphics/Metal/MetalFactory.hpp"
#include "Clove/Graphics/Metal/MetalView.hpp"

namespace garlic::clove {
	struct DeviceWrapper {
		id<MTLDevice> device{ nullptr };
		MetalView *view{ nullptr };
	};
}

namespace garlic::clove {
	MetalDevice::MetalDevice(std::any nativeWindow) {
		wrapper = std::make_unique<DeviceWrapper>();
		
		//Create device
		wrapper->device = MTLCreateSystemDefaultDevice();
		
		//Create view
		NSWindow *nsWindow{ std::any_cast<NSWindow *>(nativeWindow) };
		wrapper->view = [[MetalView alloc] initWithFrame:[nsWindow frame]];
		[wrapper->view setDevice:wrapper->device];
		
		[nsWindow setContentView:wrapper->view];
		
		factory = std::make_shared<MetalFactory>(wrapper->device, wrapper->view);
	}
	
	MetalDevice::MetalDevice(MetalDevice &&other) noexcept = default;

	MetalDevice &MetalDevice::operator=(MetalDevice &&other) noexcept = default;

	MetalDevice::~MetalDevice() {
		[wrapper->device release];
		[wrapper->view release];
	}
	
	std::shared_ptr<GhaFactory> MetalDevice::getGraphicsFactory() const {
		return factory;
	}

	void MetalDevice::waitForIdleDevice() {
		//No op
	}

	GhaDevice::Limits MetalDevice::getLimits() const {
		//TODO
		return {};
	}
}
